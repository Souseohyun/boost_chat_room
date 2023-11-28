#pragma once
#include<iostream>
#include<boost/asio.hpp>

class ChatServer;
class ChatSession;

class TcpSession :public std::enable_shared_from_this<TcpSession>{
    using tcp = boost::asio::ip::tcp;
private:
    tcp::socket socket_;
    
    std::shared_ptr<ChatSession> pChatSess_;

    ChatServer&                  chatServer_;
    boost::asio::streambuf       streamBuff_;


public:
    TcpSession(tcp::socket& socket,ChatServer& serv);
    void IintTcpSession();
    //身份验证逻辑
    void Authentication();
    void SendAuthentication();
    void ReadAuthentication();
    void ParseAuthentication(std::string&);

    //站在服务器的角度，对于此会话
    void ListeningFromCli();

    //数据缓冲区相关
    void ClearStreambuf();

};
