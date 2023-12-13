#pragma once

#include <iostream>
#include <boost/asio.hpp>
#include <nlohmann/json.hpp>
#include <variant>


class TcpSession;

class ImageSession : public std::enable_shared_from_this<ImageSession> {
    using tcp = boost::asio::ip::tcp;
private:
    std::mutex  sessionMutex_;
    //tcp::socket socket_;
    TcpSession&             tcpSession_;
    boost::asio::streambuf  buff_;



public:
    
    ImageSession() = delete;
    ImageSession(TcpSession&);


    
    void ImageSessionDownload(const std::string& request);

private:
     

    
    void PostHttpResult(const std::string&);
    void ProcessRequest(char*,std::size_t);


    void ClearStreambuf();

};