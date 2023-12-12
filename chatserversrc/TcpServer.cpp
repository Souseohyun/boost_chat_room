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
    //此处结束后，将会逐步递归回，单独一个线程异步接客

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
                    //虚函数覆写导致传入本体指针
                    std::shared_ptr<TcpSession> pTcpSess;
                    if(this->GetType() == "ChatServer"){
                        std::cout<<"Chat"<<std::endl;
                        pTcpSess = std::make_shared<TcpSession>(socket,GetChatServRef());
                        //初始化（实际目的是验证身份）
                        pTcpSess->IintChatTcpSession();
                    }else if(this->GetType() == "ImageServer"){
                        std::cout<<"Image"<<std::endl;
                        pTcpSess = std::make_shared<TcpSession>(socket,GetImageServRef());
                        //初始化（进入TcpSession中有关图像会话的逻辑
                        pTcpSess->InitImageTcpSession();
                    }
                    
                    {
                        this->tcpMutex_.lock();
                        this->sessionList_.push_back(pTcpSess);
                        this->tcpMutex_.unlock();
                    }
                    int i = 1;
                    for(auto session:sessionList_){
                        std::cout<<i++<<std::endl;
                    }
                    
                }else{
                    std::cout<<"accept fail"<<std::endl;
                }
                this->StartAccept();
            }
        );
    


    GetIOC().run();
}



void TcpServer::RemoveSession(const std::shared_ptr<TcpSession> &session){
    std::lock_guard<std::mutex> lock(tcpMutex_);
    sessionList_.remove(session);
    // 不需要显式删除 session，智能指针会自动处理
    std::cout<<"session delete"<<std::endl;
}

void TcpServer::DoBrocastMessage(const std::string &msg,const TcpSession* sender)
{
    std::lock_guard<std::mutex> lock(tcpMutex_);
    for (const auto& session : sessionList_) {
        if (session == sender->shared_from_this()) {
            continue;
        }
        session->SendDataPacket(msg);
    }
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
    
    /*暂时废弃
    // 遍历所有活跃的会话并进行心跳检查
    for (auto it = sessionList_.begin(); it != sessionList_.end(); ) {
            auto session = *it;
            if (!session->isAlive()) {
                tcpMutex_.lock();
                it = sessionList_.erase(it);
                tcpMutex_.unlock();
            } else {
                ++it;
                //std::cout<<"living"<<std::endl;
            }
    }*/
}
