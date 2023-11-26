#pragma once
#include <iostream>
#include <boost/asio.hpp> // Include the necessary header for boost::asio


class ChatServer;
/*
    先写demo，该demo直接起通信层，达到socket通信目的
    之后完善，完善阶段要求ChatSession继承TcpSession
    完善TcpSession，添加通信协议等细节
*/
class ChatSession final : public std::enable_shared_from_this<ChatSession>{
    using tcp = boost::asio::ip::tcp;
private:
    std::mutex  sessionMutex_;
    tcp::socket socket_;
    ChatServer& serv_;
    boost::asio::streambuf  buff_;

public:
    ChatSession();
    ~ChatSession() = default;
    ChatSession(const ChatSession&) = delete;
    ChatSession& operator =(const ChatSession& ) = delete;

    ChatSession(tcp::socket& ,ChatServer& );
    void InitializeSession();
    
    void SendAuthentication();
    void ReadAuthentication();
    void ParseAuthentication(std::string&);

    void ClearStreambuf();
};