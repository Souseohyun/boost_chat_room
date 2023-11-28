#include"ChatServer.hpp"

#include"../base/ConfigFileReader.hpp"

std::mutex ChatServer::serverMutex_;
std::condition_variable ChatServer::serverCond_;
//bool ChatServer::TcpServer:: = false;

ChatServer::ChatServer():TcpServer(){
    
}

//chatserver本身并不async_accept，而是Init了TcpServer
//接受链接请求交给了底层TcpServer
//TcpServer建立socket连接后，会新建一个ChatSession
//文字通信的步骤，在ChatSession中
void ChatServer::InitChatServer(std::string& ip,std::uint16_t port){
    if(!TcpServer::Init(ip,port)){
        std::cerr<<"TcpServer Init Error"<<std::endl;
    }
    this->MakeMysql();
    
}

void ChatServer::WaitForServerToStop(){
    std::unique_lock<std::mutex> lock(serverMutex_);
    //带谓词的wait，只有当lambda表达式返回true时才会就继续执行
    //而start会将状态置true，服务器不调用Stop终止，就不会满足该条件变量
    serverCond_.wait(lock, []{ return !serverRunning_; });
}

void ChatServer::MakeMysql(){
    servMysql_ = std::make_unique<BoostMysql>(GetIOC());
    ConfigFileReader config("./etc/chatserver.conf");
    std::string mysqlHost = config.GetConfigValue("mysqlhost");
    std::string mysqlPort = config.GetConfigValue("mysqlport");
    std::string mysqlUser = config.GetConfigValue("mysqluser");
    std::string mysqlPwd  = config.GetConfigValue("mysqlpassword");
    std::string mysqlDB   = config.GetConfigValue("mysqldbname");

    if(!servMysql_->Initialize(mysqlHost,mysqlPort,mysqlUser,mysqlPwd,mysqlDB)){
        //异常处理已经在函数内部实现，此处仅退出
        return;
    }
}

BoostMysql& ChatServer::GetMysql()
{
    return *servMysql_;
}

void ChatServer::Stop() {
    // 停止服务器逻辑
    {
        std::lock_guard<std::mutex> lock(serverMutex_);
        serverRunning_ = false;
    }
    //notify用于提醒main线程，醒醒，该再检查谓词是否成立了！
    serverCond_.notify_one();
    TcpServer::Stop();
    /*
    if (servThread_.joinable()) {
        servThread_.join();
    }*/
}


