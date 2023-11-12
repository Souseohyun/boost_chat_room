#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <boost/asio.hpp>
#include "session.hpp"  // 包含 Session 的头文件

class Server {
private:
    boost::asio::ip::tcp::acceptor acceptor_;
    std::vector<std::shared_ptr<Session>> sessionslist_;

public:
    Server(boost::asio::io_context& ioc, std::uint16_t port);
    void Broadcast(const std::string& message,void*);

private:
    void DoAccept();
};

#endif // SERVER_HPP
