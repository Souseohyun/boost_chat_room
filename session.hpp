#ifndef SESSION_HPP
#define SESSION_HPP

#include <vector>
#include <boost/asio.hpp>
#include <iostream>

#define BUFSIZE 1024
class Server; // 前向声明

class Session : public std::enable_shared_from_this<Session> {
private:
    boost::asio::ip::tcp::socket socket_;
    std::vector<char> buff_;//Session buffer
    Server& server_;//The server for this session
    std::string usrname_;//User name


public:
    Session(boost::asio::ip::tcp::socket sock, Server& server);
    void start();
    void Deliver(const std::string& msg);
    void* GetIdentifier();
    
private:
    void NameEntry();
    void DoBroadcast();
    void ServerListenRead();
    void DoTest();
};

#endif // SESSION_HPP
