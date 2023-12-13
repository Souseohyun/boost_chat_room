#include<fstream>

#include "../chatserversrc/TcpSession.hpp"
#include "ImageSession.hpp"
#include "../mysqlapi/BoostMysql.hpp"


ImageSession::ImageSession(TcpSession & tcpSession)
:tcpSession_(tcpSession){
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
        int pic_id = std::stoi(imageId);
        std::string baseSql = "select f_avatar_url from t_user where f_user_id = "+imageId+";";
        //测试用例，通过pic_id == f_user_id，拿到头像图片
        std::cout<<pic_id<<std::endl;

        SqlResult result = tcpSession_.UseImageMysql().ExecSql(baseSql);

        
        std::visit([this](auto&& value) {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, int>) {
            // 处理 int 类型的结果
            
            } else if constexpr (std::is_same_v<T, std::string>) {
            // 处理 string 类型的结果
            PostHttpResult(value);
            }
            // 处理其他类型
        }, result);
        //std::cout<<result<<std::endl;
        //将result回送给客户端
        //PostHttpResult(filePath);
        
    }

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
