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

    int ParseUserId(const std::string& request);
    void SendImage(int friendId, const std::string& imagePath);
    //关于GET download请求
    void ImageSessionDownload(const std::string& request);
    //关于GET getAllImages请求
    void HandleAllImagesRequest(const std::string& request);
    bool SendEndOfReply();//HandleAllImagesRequest主逻辑执行完后，发给客户端的end信号

private:
     

    
    void PostHttpResult(const std::string&);
    void ProcessRequest(char*,std::size_t);


    void ClearStreambuf();

};