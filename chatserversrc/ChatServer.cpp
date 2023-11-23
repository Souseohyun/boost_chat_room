#include"ChatServer.hpp"

ChatServer::ChatServer(boost::asio::io_context& ioc,std::uint16_t port)
:acceptor_(ioc,tcp::endpoint(tcp::v4(),port)){
    this->Run();
}

void ChatServer::Run(){
    this->DoAsyncAccept();
}

void ChatServer::DoAsyncAccept(){
    this->acceptor_.async_accept(
            [this](const boost::system::error_code& ec,tcp::socket socket){
                //将Socket交给Session创建出会话
                //及时更新cliList
                if(!ec){
                    //this指针与shared_ptr相悖，不能使用
                    //尽可能传server的引用形
                    auto pSess = std::make_shared<ChatSession>(socket,*this);
                    this->sessionList_.push_back(pSess);
                    //pcli->RunMenu();
                }else{
                    std::cout<<"accept fail"<<std::endl;
                }
                this->DoAsyncAccept();
            }
        );
}