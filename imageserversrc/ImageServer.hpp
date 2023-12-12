#pragma once

#include "../chatserversrc/TcpServer.hpp"
#include"../mysqlapi/BoostMysql.hpp"




class ImageServer : public TcpServer{
using tcp = boost::asio::ip::tcp;
public:
    //重写纯虚函数
    std::string GetType() const override { return "ImageServer";}

    ChatServer& GetChatServRef() override {
        std::cerr<<"ImageServer return ChatServer Error"<<std::endl;
        throw std::logic_error("GetImageServRef called on an inappropriate object");
    }
    ImageServer& GetImageServRef() override {
        return *this;
    }

    //ChatServer所用数据库
    std::unique_ptr<BoostMysql>             servMysql_;
    //服务器serverMutex_，条件变量
    static std::mutex                       serverMutex_;
    static std::condition_variable          serverCond_;


    //静态成员函数，用于main中等待Server结束，直接通过类名唤起
    static void WaitForServerToStop();

    void InitImageServer(std::string& ip,std::uint16_t port);
    void MakeMysql();
    BoostMysql& GetMysql();

    void Stop();
};
