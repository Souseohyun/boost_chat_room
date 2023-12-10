#include<iostream>
#include<stdlib.h>
#include<cstring>
#include<unistd.h>              //getopt()

#include"../utils/Daemon.hpp"   //DaemonRun();
#include"../base/ConfigFileReader.hpp"
#include"../base/Singleton.hpp"

#include"./ImageServer.hpp"

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

    ConfigFileReader config("./etc/imageserver.conf");
    //初始化imageserver配置
    std::string listenIP = config.GetConfigValue("listenip");
    short listenPort = static_cast<short>(std::stoi(config.GetConfigValue("listenport")));

    Singleton<ImageServer>::Instance().InitImageServer(listenIP,listenPort);


    //等待服务器结束
    ImageServer::WaitForServerToStop();
    std::cout<<"ImageServer has shut down"<<std::endl;
}