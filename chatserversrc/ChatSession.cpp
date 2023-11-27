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
#include"ChatServer.hpp"
#include "ChatSession.hpp"

ChatSession::ChatSession(tcp::socket& socket,ChatServer& serv)
:socket_(std::move(socket)),serv_(serv){
    //std::cout<<"it's test----ChatSession Success"<<std::endl;
    //socket建立成功后开始验证身份逻辑
    //SendAuthentication();切勿在构造函数中shared_from_this
    //很容易在构造函数中加载某函数，某函数不自觉使用以上
    bLive_ = true;
   
    
}
//绕开构造函数中weak error，初始化后发送身份验证请求
void ChatSession::InitializeSession() {
    SendAuthentication();
}

void ChatSession::CloseMyself(){
    serv_.removeSession(shared_from_this());
    std::lock_guard<std::mutex> lock(sessionMutex_);
    bLive_ = false;
}

bool ChatSession::isAlive() const {
    return bLive_;

/*
    return std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::steady_clock::now() - lastActiveTime_)
        .count() < heartbeatTimeout_;
        */
}

/*
    身份验证逻辑：
    1-建立好socket链接后，服务端异步读入数据
    2-客户机点击登录则主动将框内验证信息发送（账号密码）
    3-调取数据库查询，验证成功进入会话逻辑，失败中断socket链接
    (base版本：
        由于没有写客户端代码，客户机用nc命令代替
        1-服务器向客户端发送（账号/密码）请求，客户端输入。
        2-根据账号，服务端查询数据库，没有则提示错误，断开连接。
        3-有账号，服务端验证密码，正确则进入会话逻辑
        )
    */

void ChatSession::SendAuthentication(){
    auto self = shared_from_this();
    socket_.async_write_some(
        boost::asio::buffer("请输入账号@密码：\n"),
        [this,self](const boost::system::error_code& ec,std::size_t bytes){
            if(!ec && bytes!=0){
                self->ReadAuthentication();
            }else{
                self->SendAuthentication();
            }
        });

        
}

//验证信息格式：账号@密码\n（\n发送）
void ChatSession::ReadAuthentication(){
    ClearStreambuf();
    auto self = shared_from_this();
    boost::asio::async_read_until(socket_, buff_, '\n',
        [this, self](const boost::system::error_code& ec, std::size_t /*bytes_transferred*/) {
            if (!ec) {
                // 数据读取成功，处理数据
                std::istream ist(&self->buff_);
                std::string line;
                //使用 std::getline 它会读取并丢弃 \n
                std::getline(ist,line);
                ParseAuthentication(line);
            } else {
                // 错误处理
            }
        }
    );

    
}


//解析验证信息
void ChatSession::ParseAuthentication(std::string& line){
    //Windows发来的数据换行符是\r\n，getline处理了\n，也许会\r也要处理
    if(!line.empty() && line.back() == '\r')
        line.pop_back();
    //@分割账号密码
    std::size_t delimiterPos = line.find('@');
    if(delimiterPos != std::string::npos){
        std::string usrname = line.substr(0,delimiterPos);
        std::string pasword = line.substr(delimiterPos+1);

        std::cout<<"usr: "<<usrname<<"  password: "<<pasword<<std::endl;

        //在此处调用server，使用其mysql()进行核验
        if(serv_.GetMysql().CheckUserInfo(usrname,pasword)){
            //往回发送成功登陆，否则反之
            PushMessege("验证通过，登陆成功\n");
            ListeningFromCli();
        }else{
            PushMessege("验证失败，断开连接\n");
            socket_.close();

        }

    }
}

void ChatSession::PushMessege(const std::string & msg){
    auto self = shared_from_this();
    boost::asio::async_write(self->socket_,boost::asio::buffer(msg),
    [this,self](const boost::system::error_code& ec,std::size_t bytes){
        if(!ec && bytes!=0){
        }else{
            std::cerr<<"PushMessage error :"<<ec.value()<<std::endl;
        }
    });
}

//该函数目前只支持收发文字，但合理的需求是收发数据包
//根据数据包的标志位判断这是一条什么样的信息
//应该由TcpServer accept之后建立一个TcpSession
//TcpSession做底层数据包的收发判断，是文字，再交付给ChatSession负责
void ChatSession::ListeningFromCli(){
    auto self = shared_from_this();
    ClearStreambuf();  // 清空缓冲区

    // 设置预期读取的最大字节数
    std::size_t buffer_size = 1024;

    // 异步读取数据到streambuf
    boost::asio::async_read_until(socket_, buff_, '\n',
        [this, self](const boost::system::error_code& ec, std::size_t /*bytes_transferred*/) {
            if (!ec) {
                // 数据读取成功，处理数据
                std::istream ist(&self->buff_);
                std::string line;
                //使用 std::getline 它会读取并丢弃 \n
                std::getline(ist,line);
                std::cout<<line<<std::endl;
                // 继续监听
                self->ListeningFromCli();
            } else {
                // 错误处理
                self->CloseMyself();
            }
        }
    );

    
}





void ChatSession::ClearStreambuf(){

    
    buff_.consume(buff_.size());
    
    /*
    强制将buff_缓冲区缩小而不是内部管理
    boost::asio::streambuf::mutable_buffers_type bufs = streambuf.prepare(0);
    streambuf.commit(0);
    */
}