#include "TcpServer.hpp"

bool TcpServer::serverRunning_ = false;

boost::asio::io_context &TcpServer::GetIOC(){
    static boost::asio::io_context ioc;
    return ioc;
}

TcpServer::TcpServer()
:acceptor_(GetIOC()),heartbeatTimer_(GetIOC()){
    //启用心跳包
    StartHeartbeat();
}

bool TcpServer::Init(const std::string &ip, std::uint16_t port){
    boost::asio::ip::tcp::endpoint ep(boost::asio::ip::tcp::v4(),port);
    ep.address(boost::asio::ip::address::from_string(ip));
    acceptor_.open(boost::asio::ip::tcp::v4());
    acceptor_.bind(ep);
    acceptor_.listen();
    this->Start();
    return true;
}

void TcpServer::Start(){
    serverRunning_ = true;
    this->tcpServThread_ = std::thread(&TcpServer::StartAccept,this);
    //此处结束后，将会逐步递归回ChatServer中，单独一个线程异步接客

}

void TcpServer::Stop(){
    // 停止服务器逻辑

    if (tcpServThread_.joinable()) {
        tcpServThread_.join();
    }

}

void TcpServer::StartAccept(){
    

    this->acceptor_.async_accept(
            [this](const boost::system::error_code& ec,boost::asio::ip::tcp::socket socket){
                //将Socket交给Session创建出会话
                if(!ec){
                    //this指针与shared_ptr相悖，不能使用
                    //尽可能传server的引用形
                    auto pSess = std::make_shared<ChatSession>(socket,GetChatServRef());
                    {
                        this->tcpMutex_.lock();
                        this->sessionList_.push_back(pSess);
                        this->tcpMutex_.unlock();
                    }
                    
                    //初始化（实际目的是验证身份）
                    pSess->InitializeSession();
                }else{
                    std::cout<<"accept fail"<<std::endl;
                }
                this->StartAccept();
            }
        );
    


    GetIOC().run();
}

void TcpServer::removeSession(const std::shared_ptr<ChatSession> &session){
    std::lock_guard<std::mutex> lock(tcpMutex_);
    sessionList_.remove(session);
}

void TcpServer::StartHeartbeat(){
    //45s后响闹钟，异步等待，时辰到了Check并再次递归回该函数
    heartbeatTimer_.expires_after(std::chrono::seconds(heartbeatInterval_));
    heartbeatTimer_.async_wait([this](const boost::system::error_code& ec) {
        if (!ec) {
                //std::cout<<"async_wait success"<<std::endl;
                CheckHeartbeat();
                StartHeartbeat(); // 继续下一轮心跳检查
        } else {
                // 错误处理，例如日志记录
                std::cerr << "Heartbeat Timer Error: " << ec.message() << std::endl;
                StartHeartbeat();
        }
    });
}

void TcpServer::CheckHeartbeat(){
    // 遍历所有活跃的会话并进行心跳检查
    for (auto it = sessionList_.begin(); it != sessionList_.end(); /* no increment */) {
            auto session = *it;
            if (!session->isAlive()) {
                tcpMutex_.lock();
                it = sessionList_.erase(it);
                tcpMutex_.unlock();
            } else {
                ++it;
                //std::cout<<"living"<<std::endl;
            }
    }
}
