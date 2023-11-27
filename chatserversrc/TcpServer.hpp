#include <boost/asio.hpp>
#include <memory>
#include <string>
#include <list>

#include"./ChatSession.hpp"

class TcpServer{
public:
//期待派生类的覆写以获取其指针
    virtual ChatServer& GetChatServRef() = 0;

protected:
    boost::asio::ip::tcp::acceptor acceptor_;
    //运行状态
    static bool                             serverRunning_;
private:
    
    //server独立线程，不妨碍其他逻辑
    std::thread                             tcpServThread_;
    
    //session list
    std::list<std::shared_ptr<ChatSession>> sessionList_;


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
    void startAccept();
    void DoAsyncAccept();
};