#include "TcpServer.hpp"

bool TcpServer::serverRunning_ = false;

boost::asio::io_context &TcpServer::GetIOC(){
    static boost::asio::io_context ioc;
    return ioc;
}

TcpServer::TcpServer()
:acceptor_(GetIOC()){

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
    this->tcpServThread_ = std::thread(&TcpServer::startAccept,this);
    //此处结束后，将会逐步递归回ChatServer中

}

void TcpServer::Stop(){
    // 停止服务器逻辑

    if (tcpServThread_.joinable()) {
        tcpServThread_.join();
    }

}

void TcpServer::startAccept(){
    
    this->acceptor_.async_accept(
            [this](const boost::system::error_code& ec,boost::asio::ip::tcp::socket socket){
                //将Socket交给Session创建出会话
                if(!ec){
                    //this指针与shared_ptr相悖，不能使用
                    //尽可能传server的引用形
                    auto pSess = std::make_shared<ChatSession>(socket,GetChatServRef());
                    this->sessionList_.push_back(pSess);
                    //初始化（实际目的是验证身份）
                    pSess->InitializeSession();
                }else{
                    std::cout<<"accept fail"<<std::endl;
                }
                this->startAccept();
            }
        );

    GetIOC().run();
}


