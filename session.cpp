#include "session.hpp"
#include "server.hpp"

Session::Session(boost::asio::ip::tcp::socket sock, Server& server)
    : socket_(std::move(sock)), server_(server),buff_(BUFSIZE) {
    
}

void Session::start() {
    //确认发言昵称
    this->NameEntry();
    
}

void Session::NameEntry(){
    auto self(shared_from_this());
    boost::asio::write(socket_,boost::asio::buffer("Enter your nickname: "));
    socket_.async_read_some(boost::asio::buffer(buff_),
    [this,self](const boost::system::error_code& ec,std::size_t bytes_transf){
        if(!ec && bytes_transf > 0){
            //最后-1是去掉用户输入的空格
            self->usrname_ = std::string(self->buff_.begin(),self->buff_.begin()+bytes_transf-1);
            //清空缓冲区防止污染后续数据
            self->buff_.clear();
            self->buff_.resize(BUFSIZE);
            //昵称设置完之后，才能开始聊天室逻辑
            self->ServerListenRead();
        }else{}
    });
}

void Session::Deliver(const std::string& msg) {
    auto self(shared_from_this());
    //修改发言格式
    //std::string prefixedMsg = std::string("user:\n")+msg;
    //开始写
        boost::asio::async_write(
            this->socket_,
            boost::asio::buffer(msg),//此处应发送传来的msg，而非自身Session::buffer_
            [this,self](
                const boost::system::error_code& ec,
                std::size_t bytes_transf){
                    if(!ec){}
                    else{
                        //发送错误时处理逻辑
                    }
                }
        );
}

void Session::DoBroadcast() {
    auto self(shared_from_this());
    //修改消息格式：
    std::string message = usrname_ +":\n"+ std::string(buff_.begin(),buff_.end());
    
    //实际广播动作
    server_.Broadcast(message,self->GetIdentifier());
    
}

void Session::ServerListenRead() {
    //以防万一，每次read前再次清空之前的buff_
    //this->buff_.clear();
    auto self(shared_from_this());
    socket_.async_read_some(boost::asio::buffer(buff_),
        [this,self](const boost::system::error_code& ec,
                    std::size_t bytes_transf){

                if(!ec && bytes_transf > 0){
                    //执行广播逻辑，将消息分发给除自己所有的socket
                    self->DoBroadcast();
                    //每次操作完都对缓冲区进行重置，以免数据污染
                    self->buff_.clear();
                    self->buff_.resize(BUFSIZE);
                    //Debuging
                    //std::cout<<"read end "<<std::flush;
                    //DoTest();
                    

                }else{
                    
                }
                //无论是否成功，都再次将该事件派遣到IO异步执行事件表中继续侦听
                boost::asio::post(socket_.get_executor(),
                [this, self]() { self->ServerListenRead(); });
                    
        });
        //转发完信息开始继续侦听
        //ServerListenRead();   在此处写，天大错误，即使是异步，我们也需要等上一个连接建立完，在启用第二个
        //否则无限递归，直接段错误
}

void Session::DoTest(){
    std::cout<<this->buff_.data()<<std::endl;
}

void* Session::GetIdentifier() {
    return static_cast<void*>(this);
}