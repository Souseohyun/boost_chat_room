//#include"ChatSession.hpp"
/*万万不可将server.hpp包含在session.hpp中
  毫无疑问这将引起循环依赖，但我们可以巧妙地利用提前声明和其他办法绕开
  1-Server类中需要用到Session类方法，
    我们在Server.hpp中包含Session.hpp文件。
  2-Session类中也需要用到Server类方法，
    首先在Session.hpp文件中提前声明class Server;
    Session.cpp直接引入Server.hpp而不是Session.hpp;
  这样做的好处：
    提前声明使得Session.hpp中能出现Server类（虽然不能用具体方法，但谁会在hpp中写具体方法呢？）
    在后续使用中Session.cpp的编写中，由于引入了Server.hpp
    Server类得以完整的加载
*/
#include"TcpSession.hpp"
#include "ChatSession.hpp"

ChatSession::ChatSession(TcpSession& tcpSession)
:tcpSession_(tcpSession){
    //std::cout<<"it's test----ChatSession Success"<<std::endl;
    //socket建立成功后开始验证身份逻辑
    //SendAuthentication();切勿在构造函数中shared_from_this
    //很容易在构造函数中加载某函数，某函数不自觉使用以上
    bLive_ = true;
   
    
}
//绕开构造函数中weak error，初始化后发送身份验证请求
void ChatSession::InitializeSession() {
    //SendAuthentication();
}

void ChatSession::CloseMyself(){
    /*serv_.removeSession(shared_from_this());
    std::lock_guard<std::mutex> lock(sessionMutex_);
    bLive_ = false;
    */
}

bool ChatSession::isAlive() const {
    return bLive_;

/*
    return std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::steady_clock::now() - lastActiveTime_)
        .count() < heartbeatTimeout_;
        */
}



void ChatSession::ClearStreambuf(){

    
    buff_.consume(buff_.size());
    
    /*
    强制将buff_缓冲区缩小而不是内部管理
    boost::asio::streambuf::mutable_buffers_type bufs = streambuf.prepare(0);
    streambuf.commit(0);
    */
}