#define BOOST_ASIO_NO_DEPRECATED

#include"server.hpp"

int main(){
    std::uint16_t port = 2362;
    boost::asio::io_context ioc;
    Server serv(ioc,port);

    ioc.run();
    return 0;
}