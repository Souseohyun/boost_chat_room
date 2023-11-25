#include<iostream>
#include<stdlib.h>
#include<cstring>
#include<unistd.h>
#include<boost/asio.hpp>

#include"../utils/Daemon.hpp"
#include"../base/ConfigFileReader.hpp"
#include"../base/Singleton.hpp"
#include"../chatserversrc/ChatServer.hpp"

#include"../mysqlapi/DatabaseMysql.hpp"

int main(int argc,char* argv[]){


//如要守护进程启动加-d
    int ch;
    bool bdaemon = false;
    while((ch = getopt(argc,argv,"d")) != -1){
        switch (ch)
        {
        case 'd':
            bdaemon = true;
            //std::cout<<"i have -d"<<std::endl;
            DaemonRun();
            break;
        }
    }
        
    //开始读取各项配置文件
    ConfigFileReader config("./etc/chatserver.conf");

    //初始化chatserver配置
    std::string listenIP = config.GetConfigValue("listenip");
    short listenPort = static_cast<short>(std::stoi(config.GetConfigValue("listenport")));

    //初始化mysql配置
    std::string mysqlLink = config.GetConfigValue("dblink");
    std::string mysqlUser = config.GetConfigValue("dbuser");
    std::string mysqlPwd  = config.GetConfigValue("dbpassworld");
    std::string mysqlDB   = config.GetConfigValue("dbname");
    
    //起一个Server会无限期阻塞在Run()，我们可以在中另起一个线程
    /*
      单例设计模式下，父子线程共用一个ChatServer
      且io_context的建立 在thread建立之前，这意味着ioc的生命周期
      与main相同，main结束后会析构ioc，导致子线程运行不正确
      main需要条件变量wait(lock,[]{})约束并等待服务器结束! 
    */
    //init()中自含start()。会自启动
    Singleton<ChatServer>::Instance().Init(listenIP,listenPort);

    Singleton<DatabaseMysql>::Instance().Initialize(mysqlLink,mysqlUser,mysqlPwd,mysqlDB);


    //等待服务器结束
    ChatServer::WaitForServerToStop();
    std::cout<<"Server has shut down"<<std::endl;


    return 0;
}