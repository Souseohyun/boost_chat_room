#pragma once

#ifdef _MORE_CLIENT_TYPE
//通过枚举标识不同设备（此时代码并未涉及跨平台）
enum CLIENT_TYPE
{
    CLIENT_TYPE_UNKOWN,
    CLIENT_TYPE_PC,
    CLIENT_TYPE_ANDROID,
    CLIENT_TYPE_IOS,
    CLIENT_TYPE_MAC
};
#endif

#include<memory>
#include<list>
#include<map>
#include<mutex>
#include<atomic>
#include<boost/asio.hpp>

#include"./ChatSession.hpp"
#include"../mysqlapi/BoostMysql.hpp"
#include"./TcpServer.hpp"


//#define _TCP_SERVER_TEST


//已存储用户信息
struct StoredUserInfo{
    std::int32_t    userid;
    std::string     username;
    std::string     password;
    std::string     nickname;
};

class ChatServer final :public TcpServer{
using tcp = boost::asio::ip::tcp;

public:
    //重写虚函数，使基类获取派生类指针
    ChatServer& GetChatServRef() override {
        return *this;
    }

private:
#ifdef _TCP_SERVER_TEST
    tcp::acceptor                           acceptor_;
    std::list<std::shared_ptr<ChatSession>> sessionList_;
    static bool                             serverRunning_;  
    

#endif   
    //server独立线程，不妨碍其他逻辑
    std::thread                             servThread_;
    //服务器serverMutex_，条件变量
    static std::mutex                       serverMutex_;
    static std::condition_variable          serverCond_;

    //为每个Session分配在ChatServer范围内唯一一个标识id
    std::atomic_int                         sessionId_;

    //ChatServer所用数据库
    std::unique_ptr<BoostMysql>             servMysql_;

public:
    ChatServer();
    void InitChatServer(std::string&,std::uint16_t);
#ifdef _TCP_SERVER_TEST    
    ~ChatServer(){
        if(servThread_.joinable()){
            servThread_.join();
            std::cout<<"join end"<<std::endl;
        }
    }
#endif
    ChatServer(const ChatServer& rhs) = delete;
    ChatServer& operator =(const ChatServer& rhs) = delete;

    //ChatServer();

    //静态成员函数，用于main中等待Server结束，直接通过类名唤起
    static void WaitForServerToStop();
    void   Stop();  //Stop中手动cond.notify_one()
#ifdef _TCP_SERVER_TEST  
    ChatServer(boost::asio::io_context& ioc,std::uint16_t port);
    ChatServer(/*boost::asio::io_context& ioc,*/std::uint16_t port);
    bool   Init(const std::string& ,std::uint16_t );
    void   Start();
    void   Stop();  //Stop中手动cond.notify_one()

    

    static boost::asio::io_context& GetIOC();
    void Run();
    void DoAsyncAccept();


#endif


    
    
    

    void MakeMysql();
    BoostMysql& GetMysql();
};