#include "ImageServer.hpp"
#include "../base/ConfigFileReader.hpp"

std::mutex ImageServer::serverMutex_;
std::condition_variable ImageServer::serverCond_;

void ImageServer::InitImageServer(std::string &ip, std::uint16_t port){
    if(!TcpServer::Init(ip,port)){
        std::cerr<<"ImageServerIniting...TcpServer Init Error"<<std::endl;
    }else{
        std::cout<<"it's test. ImageServer Success"<<std::endl;
    }
    
}

void ImageServer::MakeMysql(){
    //有待重写，此处照搬ChatServer逻辑，仅测试
    servMysql_ = std::make_unique<BoostMysql>(GetIOC());
    ConfigFileReader config("./etc/imageserver.conf");
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

BoostMysql& ImageServer::GetMysql()
{
    return *servMysql_;
}


void ImageServer::Stop() {
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

void ImageServer::WaitForServerToStop(){
    std::unique_lock<std::mutex> lock(serverMutex_);
    //带谓词的wait，只有当lambda表达式返回true时才会就继续执行
    //而start会将状态置true，服务器不调用Stop终止，就不会满足该条件变量
    serverCond_.wait(lock, []{ return !serverRunning_; });
}