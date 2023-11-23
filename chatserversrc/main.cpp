#include<iostream>
#include<stdlib.h>
#include<cstring>
#include<unistd.h>
#include<boost/asio.hpp>

#include"../utils/Daemon.hpp"
#include"../base/ConfigFileReader.hpp"
#include"../base/Singleton.hpp"
#include"../chatserversrc/ChatServer.hpp"

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
    short listenport = static_cast<short>(std::stoi(config.GetConfigValue("listenport")));

    //std::cout<<listenIP<<"  "<<listenport<<std::endl;

    boost::asio::io_context ioc;
    std::unique_ptr<boost::asio::io_context::work>pwork{
        new boost::asio::io_context::work(ioc)
    };
    ChatServer chatServer(ioc,23610);
    pwork.reset();
    ioc.run();
    return 0;
}