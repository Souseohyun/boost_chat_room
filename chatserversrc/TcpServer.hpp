#include <boost/asio.hpp>
#include <memory>
#include <string>
#include <list>

#include"./ChatSession.hpp"

class TcpServer : std::enable_shared_from_this<TcpServer>{
public:
//期待派生类的覆写以获取其指针
    virtual ChatServer& GetChatServRef() = 0;

protected:
    //保证他们原子性的Tcp锁
    std::mutex                     tcpMutex_;
    boost::asio::ip::tcp::acceptor acceptor_;
    //运行状态
    static bool                    serverRunning_;
    //session list
    std::list<std::shared_ptr<ChatSession>> sessionList_;

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
    void removeSession(const std::shared_ptr<ChatSession>& session);

private:
    void StartHeartbeat();
    void CheckHeartbeat();
};