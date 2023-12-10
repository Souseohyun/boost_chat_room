#include<iostream>
#include<boost/asio.hpp>

class ImageSession : public std::enable_shared_from_this<ImageSession> {
    using tcp = boost::asio::ip::tcp;
private:
    tcp::socket socket_;
    enum { max_length = 1024 };     //约等于范围内#define
    char data_[max_length];

public:
    ImageSession(tcp::socket socket) : socket_(std::move(socket)) {}

    void Start() {
        DoRead();
    }

private:
    void DoRead() {
        auto self(shared_from_this());
        socket_.async_read_some(boost::asio::buffer(data_, max_length),
            [this, self](boost::system::error_code ec, std::size_t length) {
                if (!ec) {
                    ProcessRequest(data_, length);
                    // You would need to implement process_request to handle the incoming data
                    // and respond appropriately
                }
            });
    }

    void ProcessRequest(char*,std::size_t);
};