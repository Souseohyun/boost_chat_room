# 程序介绍
今网络后端的架构通常是Reactor网络模型的多线程服务架构，基于待完成事件驱动的Reactor网络模型，即使其拥有良好的线程池管理技术，由于其本身特性，也仍无法避免等待计算机将内核数据拷贝至用户空间的这段时间。
本系统基于Boost开源库，使用C++语言构建Proactor多线程网络模型的C/S架构的即时通讯系统，客户端采用QT6进行开发，交互消息格式采用nlohmann库构建json格式、http格式；
实现了即时通讯系统的基本功能包括：注册用户、用户登录、会话管理、搜索查找用户、好友/群聊分组管理、添加删除好友、本地历史消息记录与加载、离线时收到的信息与通知的管理等功能。

# 新颖特点
本系统积极拥抱新标准、新特性，编译最低需求为C++17版本；
并使用了Boost库中Boost.Asio和Boost.Mysql模块（Boost 1.83版，于2023年8月11日推出，其1.82版本首次提供了Boost.Mysql库），且乐于推动跨平台支持，所使用技术非必须不适用某平台下独立API，
采用的Boost.Asio库、Boost.Mysql库，nlohmann库，客户端采用QT6开发，代码采用CMake管理，无一例外全部支持跨平台使用。

# 本地测试环境
4.2.1开发环境
（1） 操作系统：Windows 10 Version 22H2 x64
（2） 开发软件：QT 6.2.3
（3） 需下载第三方开源库：Boost version 1.82.3、nlhomann/json version 3.10.4
（4） 编译器：MinGW 8.1.0
（5） C++版本：C++11
（6） 编译工具：CMake
（7）代码管理工具：git
4.2.2部署环境
（1） 操作系统：Windows 10 Version 22H2 x64
（2） CPU：AMD Ryzen 7 4800H with Radeon Graphics   2.90 GHz
（3） 机带RAM：16.0 GB (15.4 GB 可用)


# 运行界面
注册界面
![image](https://github.com/user-attachments/assets/5718d504-819e-4868-a5ab-e11b1408c438)

服务器接受情况
![image](https://github.com/user-attachments/assets/595d91c6-eaa8-4b81-acc8-1ba9c7856d1e)

好友添加
![image](https://github.com/user-attachments/assets/77025e9c-ac79-4e11-8907-58a6dbd4530c)
![image](https://github.com/user-attachments/assets/c5cba34b-fa19-42f7-abc1-231268fc51b5)
对方收到系统信息：
![image](https://github.com/user-attachments/assets/9eca8275-a12a-415b-837a-6869d3356d0d)


正常通讯
![image](https://github.com/user-attachments/assets/70459117-b7df-483b-8d0d-b8ecedca9561)
消息格式打包
![image](https://github.com/user-attachments/assets/6e9f2286-97e9-4ba9-94be-4c537fdf3275)





