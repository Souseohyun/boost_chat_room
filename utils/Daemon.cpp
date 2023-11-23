/*
    程序由守护进程形式运行
    Kazuki.cschn@gmail.com
*/
#include<iostream>
#include<unistd.h>
#include<fcntl.h>

void DaemonRun(){
    int pid;
    pid = fork();
    if (pid < 0)
    {
        std::cout << "fork error" << std::endl;
        exit(-1);
    }
    //父进程退出，子进程独立运行
    else if (pid > 0) {
        exit(0);
    }
    std::cout<<getpid()<<std::endl;
    setsid();
    int fd;
    fd = open("/dev/null", O_RDWR, 0);
    if (fd != -1)
    {
        dup2(fd, STDIN_FILENO);
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);
    }
    if (fd > 2)
        close(fd);

    
}