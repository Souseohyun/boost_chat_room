#pragma once
#include <boost/asio.hpp>
#include <memory>
#include <string>
#include <list>

//#include"./ChatSession.hpp"
#include"./TcpSession.hpp"


class ImageServer;
//在main中，TcpServer是被隐藏构建的，显式只构建了ChatServer
/*
    结构上来说：
    外面显示声明定义ChatServer类，而若要实现ChatServer类则需实现
    底层的TcpServer类的构建；重要的服务器逻辑还在TcpServer中
    在main中InitChatServer()中最重要的就是TcpServer::Init(ip,port)
    TcpServer中的StartAccept()被继承后ChatServer使用
    Accept会建立一个TcpSession对象，TcpSession需要与服务器交互，
    于是传入的是GetChatServRef()，ChatServ的指针
    因为这个程序在压根就没TcpServer这一独立类对象，它只是继承中继
    但ChatSession并不继承TcpSession，而是其中的一个成员指针
*/

class TcpServer : std::enable_shared_from_this<TcpServer>{
public:
//期待派生类的覆写以获取其派生类指针
    virtual ChatServer& GetChatServRef() = 0;
    virtual ImageServer& GetImageServRef() = 0;
    virtual std::string GetType() const = 0;
protected:
    //保证他们原子性的Tcp锁
    std::mutex                     tcpMutex_;
    boost::asio::ip::tcp::acceptor acceptor_;
    //运行状态
    static bool                    serverRunning_;
    //session list
    std::list<std::shared_ptr<TcpSession>> sessionList_;

private:
    //单独开辟一个线程用于接客，不妨碍其他逻辑
    std::thread                             tcpServThread_;
    //定时器，用于心跳检测
    boost::asio::steady_timer               heartbeatTimer_;
    // 心跳检查间隔，单位：秒
    const int heartbeatInterval_ = 45; 
    /*
        该心跳包机制是Demo版本，仅定期检测链接的活跃状态
        并不能维持不通信的链接形式，这需要cli和serv交互的
        心跳机制，之后再来完善该心跳机制*/

public:
    static boost::asio::io_context& GetIOC();
    TcpServer();
    ~TcpServer(){
        if(tcpServThread_.joinable()){
            tcpServThread_.join();
            std::cout<<"join end"<<std::endl;
        }
    }

    bool Init(const std::string& ip,std::uint16_t port);
    void Start();
    void Stop();
    void StartAccept();
    void DoAsyncAccept();
    
    //用于配合session类中closemyself
    void RemoveSession(const std::shared_ptr<TcpSession>& session);
    void DoBrocastMessage(const std::string&,const TcpSession*);

private:
    void StartHeartbeat();
    void CheckHeartbeat();
};