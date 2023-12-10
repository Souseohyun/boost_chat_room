//#include"ChatSession.hpp"
/*万万不可将server.hpp包含在session.hpp中
  毫无疑问这将引起循环依赖，但我们可以巧妙地利用提前声明和其他办法绕开
  1-Server类中需要用到Session类方法，
    我们在Server.hpp中包含Session.hpp文件。
  2-Session类中也需要用到Server类方法，
    首先在Session.hpp文件中提前声明class Server;
    Session.cpp直接引入Server.hpp而不是Session.hpp;
  这样做的好处：
    提前声明使得Session.hpp中能出现Server类（虽然不能用具体方法，但谁会在hpp中写具体方法呢？）
    在后续使用中Session.cpp的编写中，由于引入了Server.hpp
    Server类得以完整的加载
*/
#include"TcpSession.hpp"
#include "ChatSession.hpp"

ChatSession::ChatSession(TcpSession& tcpSession)
:tcpSession_(tcpSession){
    
    
}

//TcpSession启用，传入json，
//Chat json格式见json文件夹
void ChatSession::ChatSessionStart(const nlohmann::json &json){
//type == "message_text"
    std::string text = json.value("text","");
    //待办：区分群聊私聊

    //打包信息，并调用广播(仅测试用例)
    nlohmann::json broadcastJson;
    broadcastJson["type"] = "re_message_text";
    broadcastJson["data"] = text;

    std::string reString = broadcastJson.dump();

    tcpSession_.BrocastMessage(reString);

}

void ChatSession::ClearStreambuf()
{

    //移动内部指针达到忽略该部分数据的含义
    buff_.consume(buff_.size());
}