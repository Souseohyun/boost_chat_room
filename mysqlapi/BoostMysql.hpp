#pragma once
#include <boost/mysql/error_with_diagnostics.hpp>
#include <boost/mysql/handshake_params.hpp>
#include <boost/mysql/results.hpp>
#include <boost/mysql/tcp_ssl.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/system/system_error.hpp>
#include <iostream>
#include <string>
#include <typeinfo>
#include <variant>      //C++17

//#define _BOOSTMYSQL

// 定义一个 variant  包含所有ExecSql可能的返回类型
using SqlResult = std::variant<int, std::string /* 将来的其他类型 */>;

class BoostMysql{
    using tcp = boost::asio::ip::tcp;
    struct DatabaseInfo{
        std::string host;
        std::string port;
        std::string usr;
        std::string pass;
        std::string dbname;
    };
private:
    DatabaseInfo                     info_;
    boost::asio::ssl::context        sslIoc_;
    boost::mysql::tcp_ssl_connection conn_;
    boost::mysql::statement          stmt_;
    boost::mysql::results            result_;
    bool        bInit_;

    
public:
    BoostMysql();
    ~BoostMysql();
    BoostMysql(boost::asio::io_context&);
    bool Initialize(
        std::string&,std::string&,
        std::string&,std::string&,std::string&);
    bool CheckUserInfo(std::string& usr,std::string& pass);

    SqlResult ExecSql(const std::string& sql);
    //其中processResult由外界lambda表达式定义
    
private:
    

    void disconnect();
};






