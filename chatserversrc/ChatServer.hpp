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

//已存储用户信息
struct StoredUserInfo{
    std::int32_t    userid;
    std::string     username;
    std::string     password;
    std::string     nickname;
};

class ChatServer final : public std::enable_shared_from_this<ChatServer>{
    using tcp = boost::asio::ip::tcp;
private:
    tcp::acceptor                           acceptor_;
    std::list<std::shared_ptr<ChatSession>> sessionList_;
    //服务器serverMutex_，条件变量
    static std::mutex                       serverMutex_;
    static std::condition_variable          serverCond_;
    static bool                             serverRunning_;
    //为每个Session分配在ChatServer范围内唯一一个标识id
    std::atomic_int                         sessionId_;
    
    std::thread                             servThread_;

public:
    ChatServer();
    ~ChatServer(){
        if(servThread_.joinable()){
            servThread_.join();
            std::cout<<"join end"<<std::endl;
        }
    }

    ChatServer(const ChatServer& rhs) = delete;
    ChatServer& operator =(const ChatServer& rhs) = delete;

    ChatServer(boost::asio::io_context& ioc,std::uint16_t port);
    ChatServer(/*boost::asio::io_context& ioc,*/std::uint16_t port);

    bool   Init(const std::string& ,std::uint16_t );
    void   Start();
    void   Stop();  //Stop中手动cond.notify_one()

    //静态成员函数，用于main中等待Server结束
    static void WaitForServerToStop();

    static boost::asio::io_context& GetIOC();
    void Run();
    void DoAsyncAccept();

};