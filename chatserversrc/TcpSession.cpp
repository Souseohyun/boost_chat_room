#include"TcpSession.hpp"
#include"ChatServer.hpp"

TcpSession::TcpSession(tcp::socket& socket, ChatServer &serv)
:socket_(std::move(socket)),chatServer_(serv){
    pChatSess_ = std::make_shared<ChatSession>(*this);
}

void TcpSession::IintTcpSession(){
    //初始化后开始身份验证逻辑
    std::cout<<"initing"<<std::endl;
    Authentication();
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
void TcpSession::Authentication()
{
    SendAuthentication();
}

void TcpSession::SendAuthentication()
{
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

void TcpSession::ReadAuthentication()
{
    ClearStreambuf();
    auto self = shared_from_this();
    boost::asio::async_read_until(socket_, streamBuff_, '\n',
        [this, self](const boost::system::error_code& ec, std::size_t /*bytes_transferred*/) {
            if (!ec) {
                // 数据读取成功，处理数据
                std::istream ist(&self->streamBuff_);
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

void TcpSession::ParseAuthentication(std::string &line)
{
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
        if(chatServer_.GetMysql().CheckUserInfo(usrname,pasword)){
            //往回发送成功登陆，否则反之
            //this->pChatSess_->PushMessege("验证通过，登陆成功\n");
            std::cout<<"Autn Success"<<std::endl;
            ListeningFromCli();
        }else{
            //this->pChatSess_->PushMessege("验证失败，断开连接\n");
            std::cout<<"Autn Error"<<std::endl;
            socket_.close();

        }

    }
}

//现在是demo，发来的总是文字，之后写好客户端后，
//将会打包信息，该函数将改写为根据标志位处理信息包
void TcpSession::ListeningFromCli()
{
    auto self = shared_from_this();
    ClearStreambuf();  // 清空缓冲区

    // 设置预期读取的最大字节数
    std::size_t buffer_size = 1024;

    // 异步读取数据到streambuf
    boost::asio::async_read_until(socket_, streamBuff_, '\n',
        [this, self](const boost::system::error_code& ec, std::size_t /*bytes_transferred*/) {
            if (!ec) {
                // 数据读取成功，处理数据
                std::istream ist(&self->streamBuff_);
                std::string line;
                //使用 std::getline 它会读取并丢弃 \n
                std::getline(ist,line);
                std::cout<<line<<std::endl;
                BrocastMessage(line);//仅供测试，转发给其他所有客户端

                // 继续监听
                self->ListeningFromCli();
            } else {
                // 错误处理
                //self->CloseMyself();
            }
        }
    );
}


//调取服务器协助完成广播信息
void TcpSession::BrocastMessage(const std::string& msg){
    this->chatServer_.DoBrocastMessage(msg,this);

}

void TcpSession::SendDataPacket(const std::string &data)
{
    std::cout<<"into SendDataPacket func()"<<std::endl;
    boost::asio::async_write(socket_, boost::asio::buffer(data+"\n"),
        [this,data](const boost::system::error_code& ec, std::size_t /*bytes_transferred*/) {
            if (!ec) {
                // 数据发送成功
                // 可以在这里进行一些处理
                std::cout<<"send success: "<<data<<std::endl;
            } else {
                // 错误处理
                std::cerr<<"SendDataPacket Error: "<<ec.what()<<std::endl;
            }
        }
    );
}

void TcpSession::ClearStreambuf()
{
    streamBuff_.consume(streamBuff_.size());
}
