#pragma once
#include<mysql_connection.h>
#include<mysql_driver.h>
#include<mysql_error.h>
#include<cppconn/statement.h>
#include<cppconn/resultset.h>

//(for check Mysql of init())
//#define _TEST

class DatabaseMysql{
public:
    struct DatabaseInfo{
        std::string strLink_;
        std::string strUser_;
        std::string strPwd_;
        std::string strSchema_;
    };
private:
    DatabaseInfo                        dbInfo_;
    sql::mysql::MySQL_Driver*           driver_;
    sql::Connection*                    con_;
    sql::Statement*                     stmt_;
    sql::ResultSet*                     result_;
    bool                                bInit_;

public:
    DatabaseMysql();
    ~DatabaseMysql();

    bool Initialize(const std::string& link,const std::string& user,const std::string& password,const std::string& dbname);

};