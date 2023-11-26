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

//#define _BOOSTMYSQL


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
private:
    

    void disconnect();
};
