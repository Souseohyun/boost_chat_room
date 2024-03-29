#pragma once
#include<iostream>
#include<boost/asio.hpp>
#include <nlohmann/json.hpp>



class ChatServer;
class ChatSession;
class ImageServer;
class ImageSession;
class BoostMysql;
struct FriendInfo;

class TcpSession :public std::enable_shared_from_this<TcpSession>{
    using tcp = boost::asio::ip::tcp;
private:
    int         userId_{0};
    std::function<void(int)> onAuthenticatedCallback_; // 身份验证回调

    tcp::socket socket_;
    
    std::shared_ptr<ChatSession> pChatSess_;
    std::shared_ptr<ImageSession> pImageSess_;

    ChatServer&                  chatServer_;
    ImageServer&                 imageServer_;
    
    boost::asio::streambuf       streamBuff_;

    //跟踪会话的最后活动时间(每当会话接收到数据或发送数据时，更新这个时间戳。)
    std::chrono::steady_clock::time_point lastActivityTime_;
    // 在处理接收或发送操作时调用 UpdateLastActivity()


public:
    TcpSession(tcp::socket& socket,ChatServer& serv);
    TcpSession(tcp::socket& socket,ImageServer& serv);
    void IintChatTcpSession();
    void InitImageTcpSession();
    //身份验证逻辑（Demo）
    void Authentication();
    void SendAuthentication();
    void ReadAuthentication();
    
    //身份验证逻辑（New）
    void LoginAuthen();
    void ParseAuthentication(std::string &usrname,std::string& pasword);
    void SendLoginResponse(bool bLogin,int&);
    void SendLoginResponse(bool bLogin, int& user_id, const std::vector<FriendInfo>& friends);

    
    void SetOnAuthenticatedCallback(const std::function<void(int)>& callback);

    void NotifyAuthenticationSuccess(int userId);
    


    //关于在TcpSession中调用Server的数据库进行操作
    BoostMysql& UseImageMysql();
    
    //关于会话存活性检测（心跳）
    void UpdateLastActivity();

    //站在Session的服务端的角度，对于此会话
    void ListeningFromCli();
    void ListenHandle(const boost::system::error_code& ec,std::size_t bytes);
    void BrocastMessage(const std::string&);

    //单次收发数据包
    void SendDataPacket(const std::string&);
    void ReadDataPacket();
    


    //ImageSession相关，站在图像服务器角度对于该会话的功能
    void ImageListeningFromCli();
    void ImageListenHandle(const boost::system::error_code& ec,std::size_t bytes);

    int GetUserId() const;
    ChatServer& GetServer() const;
    //数据缓冲区相关
    void ClearStreambuf();
    //必要的清理资源，如socket已断开，该Session已无存在意义
    void ClearMyself();
};
