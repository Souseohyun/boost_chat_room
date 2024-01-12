#include"ChatServer.hpp"
#include"TcpSession.hpp"
#include"../imageserversrc/ImageSession.hpp"
#include"../imageserversrc/ImageServer.hpp"

TcpSession::TcpSession(tcp::socket& socket, ChatServer &serv)
:socket_(std::move(socket)),
chatServer_(serv),
imageServer_((ImageServer&)serv){//只要不用imageServer_，一切都会好起来的，汗颜
    pChatSess_ = std::make_shared<ChatSession>(*this);
    //开启tcp keepalive机制
    socket_.set_option(boost::asio::socket_base::keep_alive(true));
}

TcpSession::TcpSession(tcp::socket &socket, ImageServer &serv)
:socket_(std::move(socket)),
imageServer_(serv),
chatServer_((ChatServer&)serv){
    pImageSess_ = std::make_shared<ImageSession>(*this);
    
    //开启tcp keepalive机制
    socket_.set_option(boost::asio::socket_base::keep_alive(true));
}

void TcpSession::IintChatTcpSession(){
    //初始化后开始身份验证逻辑
    std::cout<<"class TcpSession for ChatTcpSession init"<<std::endl;
    //Authentication();
    LoginAuthen();
}

void TcpSession::InitImageTcpSession(){
    //log by 12/11 1:04 
    std::cout<<"class TcpSession for ImageTcpSession init"<<std::endl;
    //在客户端中，只有完成ChatTcpSession的LoginAuthen()验证才能初始化图像Session
    //故不再验证身份，直接开始监听并完善http逻辑
    ImageListeningFromCli();

}

/*
    待完成工作：客户端发来密码应hash加密，服务器解密后验证
*/
void TcpSession::LoginAuthen(){
    ClearStreambuf();
    auto self = shared_from_this();
    boost::asio::async_read_until(socket_, streamBuff_, "\r\n",
            [this, self](const boost::system::error_code& ec, std::size_t /*bytes_transferred*/) {
                if (!ec) {
                    // 从streamBuff_中提取字符串
                    std::istream stream(&streamBuff_);
                    std::string str((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());

                    // 解析 JSON 数据
                    try {
                        auto json = nlohmann::json::parse(str);
                        if(json["type"] == "login"){
                        // 处理登录逻辑
                        std::string username = json["username"];
                        std::string password = json["password"];
                        // 对 username 和 password 进行验证

                        std::cout<<"user:"<<username<<"  password:"<<password<<std::endl;
                        self->ParseAuthentication(username,password);

                        }else{
                            std::cerr<<"Read json type is not Login"<<std::endl;
                            self->ClearMyself();
                        }
                        
                    } catch (const std::exception& e) {
                        // JSON 解析错误
                        std::cerr << "LoginAuthen JSON parsing error: " << e.what() << std::endl;
                    }
                } else {
                    // 错误处理
                    if (ec == boost::asio::error::eof || ec == boost::asio::error::connection_reset || ec == boost::system::errc::connection_reset) {
                        std::cout << "LoginAuthen KeepAlive Over: " << ec.what() << std::endl;
                        self->ClearMyself();
                    } else {
                        // 其他错误处理
                        std::cerr << "LoginAuthen Another Error: " << ec.what() << std::endl;
                    }
                }
            }
        );
}

void TcpSession::ParseAuthentication(std::string &usrname,std::string& pasword)
{
    //在此处调用server，使用其mysql()进行核验
        if(chatServer_.GetMysql().CheckUserInfo(usrname,pasword)){
            //往回发送成功登陆，否则反之
            //this->pChatSess_->PushMessege("验证通过，登陆成功\n");
            std::cout<<"Autn Success"<<std::endl;

            
            //查询该username对应的user_id
            //select f_user_id from t_user where f_username = 'happycat';
            
            SqlResult result_id = chatServer_.GetMysql().ExecSql(
                "select f_user_id from t_user where f_username = '"+usrname+"';"
            );
            
            std::visit([this](auto&& value) {
                using T = std::decay_t<decltype(value)>;
                if constexpr (std::is_same_v<T, int>) {
                // 处理value为 int 类型的结果
                // 进一步获取该value为id的用户的 好友信息
                std::vector<FriendInfo> friends = chatServer_.GetMysql().GetFriends(value);
                // 发送登录响应和好友信息
                for(const auto& a:friends){
                    std::cout<<"_DEBUG_FOR_GETFRIENDS"<<"\n"<<
                               a.friend_id<<" "<<a.teamname<<" "<<a.markname<<std::endl;
                }
                SendLoginResponse(true, value, friends);
                // 通知身份验证成功
                NotifyAuthenticationSuccess(value); 
                } else if constexpr (std::is_same_v<T, std::string>) {
                // 处理 string 类型的结果(不可能)
                std::cerr<<"variant visit error"<<std::endl;
                }
                // 处理其他类型(不可能)
            }, result_id);
            

            
            //ListeningFromCli();
        }else{
            //this->pChatSess_->PushMessege("验证失败，断开连接\n");
            std::cout<<"Autn Error"<<std::endl;
            socket_.close();

        }
    
}

void TcpSession::SendLoginResponse(bool bLogin, int& user_id, const std::vector<FriendInfo>& friends) {
    nlohmann::json response;
    response["login_success"] = bLogin;
    if (bLogin) {
        response["message"] = "Authentication successful.";
        response["user_id"] = user_id;

        // 添加好友信息（这是一个安全隐患，如果用户使用量大，他的好友量很多，可能这样传输会出问题，需要分段）
        nlohmann::json jsonFriends = nlohmann::json::array();
        for (const auto& friendInfo : friends) {
            nlohmann::json jsonFriend;
            jsonFriend["friend_id"] = friendInfo.friend_id;
            jsonFriend["teamname"] = friendInfo.teamname;
            jsonFriend["markname"] = friendInfo.markname;
            jsonFriends.push_back(jsonFriend);
        }
        response["friends"] = jsonFriends;
    } else {
        response["message"] = "Authentication failed. Check username or password.";
    }

    // 将 JSON 对象转换为字符串并发送响应
    std::string responseStr = response.dump();
    std::cout<<responseStr<<std::endl;
    SendDataPacket(responseStr);

    // 启动监听
    ListeningFromCli();
}


/* log 2024.1.7 16:04
void TcpSession::SendLoginResponse(bool bLogin,int& user_id){
    nlohmann::json response;
        response["login_success"] = bLogin;
        if (bLogin) {
            response["message"] = "Authentication successful.";
            // 可以添加更多的响应数据，如用户信息、令牌等
            //发回供客户端使用的该账户标识信息
            response["user_id"] = user_id;
            //chatServer_.GetMysql().ExecSql()
        } else {
            response["message"] = "Authentication failed. Check username or password.";
        }

        // 将 JSON 对象转换为字符串
        std::string responseStr = response.dump();

        

        // 发送响应
        SendDataPacket(responseStr);

        
        //启动监听
        ListeningFromCli();
}
*/




void TcpSession::SetOnAuthenticatedCallback(const std::function<void(int)>& callback) {
    onAuthenticatedCallback_ = callback;
    
}

//当验证成功时，唤醒对应回调函数
void TcpSession::NotifyAuthenticationSuccess(int userId) {
    userId_ = userId;
    if (onAuthenticatedCallback_) {
        onAuthenticatedCallback_(userId);
    }
}

//方便imagesession使用mysql
BoostMysql& TcpSession::UseImageMysql()
{
    
    //return imageServer_.GetMysql().ExecSql<std::string>(sql);
    return imageServer_.GetMysql();
}

// 将最后活动时间改为now就可以
void TcpSession::UpdateLastActivity(){
    lastActivityTime_ = std::chrono::steady_clock::now();
}



void TcpSession::ListeningFromCli()
{
    auto self = shared_from_this();
    ClearStreambuf();  // 清空缓冲区

    // 设置预期读取的最大字节数
    std::size_t buffer_size = 1024;

    // 异步读取数据到streambuf
    boost::asio::async_read_until(socket_, streamBuff_, "\r\n",
        std::bind(&TcpSession::ListenHandle, shared_from_this(), std::placeholders::_1, std::placeholders::_2)
    );

}

void TcpSession::ListenHandle(const boost::system::error_code& ec,std::size_t bytes){
    auto self = shared_from_this();
    if(!ec){
        std::istream ist(&streamBuff_);
        std::string line;
        std::getline(ist, line);//从streamBuff_拿到数据存到line里
        try {
            // 解析JSON数据
            nlohmann::json data = nlohmann::json::parse(line);

            // 根据键type拿到对应值
            std::string type = data.value("type", "");
            
            if (type == "message_text") {
                //如果是文字信息，交付给chatSession处理
                std::cout<<data<<std::endl;
                //收到之后，我们在这个函数中拿到dest，src，发给对应的客户端
                //现在当务之急是：编写一个函数，通过dest发给对应客户端
                pChatSess_->ChatSessionStart(data);

            } else if (type == "message_image") {
                
            }
            // ...更多类型的处理...

            self->ListeningFromCli();
        } catch (const nlohmann::json::parse_error& e) {
            // JSON解析错误处理
            std::cerr << "JSON parsing error: " << e.what() << std::endl;
        }
    }
}

//调取服务器协助完成广播信息
void TcpSession::BrocastMessage(const std::string& msg){
    this->chatServer_.DoBrocastMessage(msg,this);

}

//只发送string，不负责string打包成json
//若要调用该函数，需在不同业务逻辑里将json转成string再调
void TcpSession::SendDataPacket(const std::string &data)
{
    std::cout<<"into SendDataPacket func()"<<std::endl;
    std::string package = data + "\r\n";
    boost::asio::async_write(socket_, boost::asio::buffer(package),
        [this,package](const boost::system::error_code& ec, std::size_t /*bytes_transferred*/) {
            if (!ec) {
                // 数据发送成功
                // 可以在这里进行一些处理
                std::cout<<"send success: "<<std::endl;

            } else {
                // 错误处理
                //如果因为keepalive机制检测eof断连
                if (ec == boost::asio::error::eof || ec == boost::asio::error::connection_reset || ec == boost::system::errc::connection_reset) {
                    std::cout<<"SendDataPacket KeepAlive Over"<<ec.what()<<std::endl;
                    this->ClearMyself();
                }else{
                    // 其他错误处理
                    std::cerr<<"SendDataPacket Another Error"<<std::endl;
                }
            }
        }
    );
}



//Image服务器通过收发http报文完成图像的传输
void TcpSession::ImageListeningFromCli(){
    
    auto self = shared_from_this();
    ClearStreambuf();  // 清空缓冲区
    boost::asio::async_read_until(socket_, streamBuff_, "\r\n\r\n",
        std::bind(&TcpSession::ImageListenHandle, shared_from_this(), std::placeholders::_1, std::placeholders::_2)
    );

}

void TcpSession::ImageListenHandle(const boost::system::error_code &ec, std::size_t bytes)
{
    
    auto self = shared_from_this();
    if(!ec){
        std::istream stream(&streamBuff_);
        std::string request((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
        // 请求处理逻辑
        //若客户端发来的是HTTP GET命令，路径为/download请求

        //debug--
        std::cout<<"into ImageListenHandle"<<std::endl;
        //std::cout<<request<<std::endl;
        if (request.find("GET /download") != std::string::npos) {
            std::cout<<"it's Get download"<<std::endl;
            self->pImageSess_->ImageSessionDownload(request);
            
            //继续监听图片请求
            ImageListeningFromCli();
        }else if (request.find("GET /getAllImages") != std::string::npos){
            std::cout<<"it's Get getAllImages"<<std::endl;
            self->pImageSess_->HandleAllImagesRequest(request);
        }
    }

}

int TcpSession::GetUserId() const
{
    return this->userId_;
}

ChatServer &TcpSession::GetServer() const
{
    return this->chatServer_;
}

void TcpSession::ClearStreambuf()
{
    streamBuff_.consume(streamBuff_.size());
}

void TcpSession::ClearMyself(){
    // 如果 socket 仍然开放，再关闭它（基本不可能）
    if (socket_.is_open()) {
        boost::system::error_code ec;
        socket_.close(ec);
        if (ec) {
            std::cerr << "Error closing socket: " << ec.message() << std::endl;
        }
    }

    // 清理其他资源...

    // 通知 TcpServer 移除此 session
    chatServer_.RemoveSession(shared_from_this());
}
