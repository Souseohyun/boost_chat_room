#include<fstream>

#include "../chatserversrc/TcpSession.hpp"
#include "ImageSession.hpp"
#include "../mysqlapi/BoostMysql.hpp"


ImageSession::ImageSession(TcpSession & tcpSession)
:tcpSession_(tcpSession){
}

int ImageSession::ParseUserId(const std::string& request) {
    size_t idPos = request.find("id=");
    if (idPos != std::string::npos) {
        size_t idEnd = request.find(" ", idPos);
        std::string userIdStr = request.substr(idPos + 3, idEnd - (idPos + 3));
        try {
            return std::stoi(userIdStr);
        } catch (const std::invalid_argument& e) {
            std::cerr << "Invalid argument: " << e.what() << '\n';
        } catch (const std::out_of_range& e) {
            std::cerr << "Out of range error: " << e.what() << '\n';
        }
    }
    return -1; // 返回一个错误标志，表示未能成功解析用户 ID
}

//被调用处理HTTP GET /download命令
/*  request格式     根据id请求头像
GET /download?id=123 HTTP/1.1
Host: 172.30.229.221:8000
Connection: Keep-Alive
Accept-Encoding: gzip, deflate
Accept-Language: zh-CN,en,*
User-Agent: Mozilla/5.0
*/
void ImageSession::ImageSessionDownload(const std::string &request)
{
    size_t idPos = request.find("id=");
    if (idPos != std::string::npos) {
        size_t idEnd = request.find(" ", idPos);
        std::string imageId = request.substr(idPos + 3, idEnd - (idPos + 3));
        
        std::string baseSql = "select f_avatar_url from t_user where f_user_id = "+imageId+";";
        //测试用例，通过pic_id == f_user_id，拿到头像图片
        std::cout<<imageId<<std::endl;

        SqlResult result = tcpSession_.UseImageMysql().ExecSql(baseSql);

        
        std::visit([this](auto&& value) {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, int>) {
            // 处理 int 类型的结果
            
            } else if constexpr (std::is_same_v<T, std::string>) {
            // 处理 string 类型的结果
                //std::cout<<"into visit is same string"<<std::endl;
                this->PostHttpResult(value);
            }
            // 处理其他类型
        }, result);
        //std::cout<<result<<std::endl;
        //将result回送给客户端
        //PostHttpResult(filePath);
        
    }

}

void ImageSession::HandleAllImagesRequest(const std::string &request)
{
    std::cout<<request<<std::endl;
    // 解析用户 ID
    int userId = ParseUserId(request);
    if (userId == -1) {
        std::cerr << "Failed to parse user ID from request" << std::endl;
        return;
    }
    std::cout<<"in request id == "<<userId<<std::endl;

    // 获取该用户的所有好友ID
    std::vector<int> friendIds = tcpSession_.UseImageMysql().GetAllFriendIds(userId);

    

    // 遍历好友列表并发送每个好友的头像
    for (int friendId : friendIds) {
        std::cout<<"friend id:"<<friendId<<std::endl;
        std::string imagePath = tcpSession_.UseImageMysql().GetFriendImagePath(friendId);
        SendImage(friendId, imagePath);
    }

    if(SendEndOfReply())
        std::cout<<"SendEndOfReply..."<<std::endl;
}

bool ImageSession::SendEndOfReply()
{
    std::string endReply = "HTTP/1.1 204 No Content\r\n"
                            "Friend-Id: -2\r\n"  // 使用 Friend-Id: -2 作为结束标志
                            "\r\n";

    tcpSession_.SendDataPacket(endReply);
    return false;
}

void ImageSession::SendImage(int friendId, const std::string& imagePath) {
    std::cout<<"into SendImage,now image path : "<<imagePath<<std::endl;
    // 读取图片文件
    std::ifstream file(imagePath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Failed to open image file: " << imagePath << std::endl;
        return;
    }
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<char> buffer(size);
    if (!file.read(buffer.data(), size)) {
        std::cerr << "Failed to read image file: " << imagePath << std::endl;
        return;
    }

    // 构建 HTTP 响应
    std::ostringstream oss;
    oss << "HTTP/1.1 200 OK\r\n";
    oss << "Content-Type: image/jpeg\r\n";
    oss << "Content-Length: " << size << "\r\n";
    oss << "Friend-Id: " << friendId << "\r\n"; // 添加好友 ID 标头
    oss << "\r\n";
    oss.write(buffer.data(), size);

    // 发送响应
    std::string response = oss.str();
    //std::cout<<"over response string:"<<response<<std::endl;
    tcpSession_.SendDataPacket(response);
}


void ImageSession::PostHttpResult(const std::string &filePath)
{
    std::string file_path = filePath; 
    std::ifstream file(file_path, std::ios::binary | std::ios::ate);
    if (file.is_open()) {
            std::streamsize size = file.tellg();
            file.seekg(0, std::ios::beg);
            std::vector<char> buffer(size);
            if (file.read(buffer.data(), size)) {
                std::ostringstream oss;
                oss << "HTTP/1.1 200 OK\r\n";
                oss << "Content-Type: image/jpeg\r\n";
                oss << "Content-Length: " << size << "\r\n";
                oss << "Content-Disposition: attachment; filename=\"image.jpg\"\r\n";
                oss << "\r\n";
                //头部手动填充完，write写入数据部分
                oss.write(buffer.data(), size);
                std::string response = oss.str();

                tcpSession_.SendDataPacket(response);
            }
        }
}

void ImageSession::ProcessRequest(char *, std::size_t)
{
    std::cout<<"into Process Request!"<<std::endl;
}







void ImageSession::ClearStreambuf(){
    //移动内部指针达到忽略该部分数据的含义
    buff_.consume(buff_.size());
}
