#include "server.hpp"

Server::Server(boost::asio::io_context& ioc, std::uint16_t port)
    : acceptor_(ioc, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)) {
    DoAccept();
}

void Server::Broadcast(const std::string& message,void* send_id) {
    for (auto& session : sessionslist_) {
        if(session->GetIdentifier() != send_id)
            session->Deliver(message);
    }
}

void Server::DoAccept() {
    this->acceptor_.async_accept(
            [this](const boost::system::error_code& ec,boost::asio::ip::tcp::socket socket){
                //此时socket为建立好链接的socket
                //更新sessionslist，并启用会话
                auto new_session = std::make_shared<Session>(std::move(socket),*this);
                sessionslist_.push_back(new_session);
                new_session->start();
                printf("Accept Success!\n");
                //每次成功后再次侦听socket链接请求
                this->DoAccept();
            
            }
        );
        //再次侦听socket链接请求
        //this->DoAccept();  在此处写，将不等accept成功则继续执行，反复如此，栈溢出
}
