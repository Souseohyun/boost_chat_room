#include"BoostMysql.hpp"

BoostMysql::~BoostMysql()
{
    disconnect();
}

BoostMysql::BoostMysql(boost::asio::io_context &ioc)
    : sslIoc_(boost::asio::ssl::context::tls_client),
      conn_(ioc, sslIoc_)
{
}

bool BoostMysql::Initialize(std::string &host, std::string &port, std::string &usr, std::string &pass,std::string& dbname){

    if(bInit_)
        return false;
    //端点参数填入,port只接受数字
    tcp::endpoint ep(
        boost::asio::ip::address::from_string(host),
        std::stoi(port)
    );
    //数据库参数填入
    boost::mysql::handshake_params params(
        usr,pass,dbname
    );
    
    //链接mysql
    try{
        bInit_ = true;
        conn_.connect(ep,params);
#ifdef _BOOSTMYSQL
        boost::mysql::results result;
        conn_.execute("select * from t_user",result);
        std::cout<<result.rows().at(1).at(3)<<std::endl;
#endif
    }catch(const boost::system::system_error& ec){
        bInit_ = false;
        std::cerr<< "Failed to connect Mysql: " << ec.what() << std::endl;
        return false;
    }
    //更新info信息
    info_.host = host;
    info_.port = port;
    info_.usr  = usr;
    info_.pass = pass;
    info_.dbname   =  dbname;
    return true;
}

bool BoostMysql::CheckUserInfo(std::string &usr, std::string &pass)
{
    
    try {
        //带参的预处理语句
        stmt_ = conn_.prepare_statement(
            "select count(*) from t_user where f_username = ? and f_password = ?"
        );
        conn_.execute(stmt_.bind(usr,pass),result_);
        // 判断结果
        if(result_.rows().at(0).at(0).get_int64() != 0){
            //std::cout<<"you are fuking success."<<std::endl;
            return true;
        }
    } catch (const boost::system::system_error& e) {
        std::cerr << "Error querying user info: " << e.what() << std::endl;
    }

    return false; 
}




void BoostMysql::disconnect(){
    conn_.close();
    bInit_ = false;
}
