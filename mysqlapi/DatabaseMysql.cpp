#include"DatabaseMysql.hpp"

DatabaseMysql::DatabaseMysql(){
    //初始化时仅拿到MySQL驱动实例
    driver_ = sql::mysql::get_driver_instance();
    bInit_  = false;
}

bool DatabaseMysql::Initialize(
    const std::string& link,const std::string& user,
    const std::string& password,const std::string& dbname){

    if(bInit_)
        return false;
    
    //链接
    con_ = driver_->connect(link,user,password);
    //指定数据库
    con_->setSchema(dbname);
    //创立statement
    stmt_ = con_->createStatement();

    dbInfo_.strLink_ = link;
    dbInfo_.strUser_ = user;
    dbInfo_.strPwd_  = password;
    dbInfo_.strSchema_  =  dbname;

    if(!con_->isClosed()){
        stmt_->execute("set names utf8");
        bInit_ = true;
#ifdef _TEST
        std::unique_ptr<sql::ResultSet> res(stmt_->executeQuery("SELECT * FROM t_user"));
        while (res->next()) {
            std::cout<<"User ID: "<<res->getInt("f_user_id")<<
            "  Username: "<<res->getString("f_username")<<std::endl;
        }
#endif
    }else
        return false;

    return true;
    

}