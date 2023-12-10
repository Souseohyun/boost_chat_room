#pragma once
#include <iostream>
#include <boost/asio.hpp> // Include the necessary header for boost::asio
#include <nlohmann/json.hpp>

class TcpSession;
/*
    先写demo，该demo直接起通信层，达到socket通信目的
    之后完善，完善阶段要求ChatSession继承TcpSession
    完善TcpSession，添加通信协议等细节
*/
class ChatSession final : public std::enable_shared_from_this<ChatSession>{
    using tcp = boost::asio::ip::tcp;
private:
    std::mutex  sessionMutex_;
    //tcp::socket socket_;
    TcpSession&             tcpSession_;
    boost::asio::streambuf  buff_;



public:
    ChatSession();
    ~ChatSession() = default;
    ChatSession(const ChatSession&) = delete;
    ChatSession& operator =(const ChatSession& ) = delete;

    ChatSession(TcpSession& );

    //核心业务逻辑，与TcpSession交互
    void ChatSessionStart(const nlohmann::json& json);

    void ClearStreambuf();

};