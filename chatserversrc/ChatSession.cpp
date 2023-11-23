#include"ChatSession.hpp"

ChatSession::ChatSession(tcp::socket& socket,ChatServer& serv)
:socket_(std::move(socket)),serv_(serv){
    std::cout<<"it's test----ChatSession Success"<<std::endl;
}