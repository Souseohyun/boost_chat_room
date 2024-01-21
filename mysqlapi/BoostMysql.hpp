#pragma once
#include <boost/mysql/error_with_diagnostics.hpp>
#include <boost/mysql/handshake_params.hpp>
#include <boost/mysql/results.hpp>
#include <boost/mysql/tcp_ssl.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/system/system_error.hpp>
#include <iostream>
#include <string>
#include <typeinfo>
#include <variant>      //C++17

//#define _BOOSTMYSQL

// 定义一个 variant  包含所有ExecSql可能的返回类型
using SqlResult = std::variant<int, std::string /* 将来的其他类型 */>;

/*根据f_user_id = 1003查找好友关系的长sql语句，返回一个结果集：
SELECT 
    CASE 
        WHEN f_user_id1 = 1003 THEN f_user_id2 
        ELSE f_user_id1 
    END AS friend_id,
    CASE 
        WHEN f_user_id1 = 1003 THEN f_user2_teamname 
        ELSE f_user1_teamname 
    END AS teamname,
    CASE 
        WHEN f_user_id1 = 1003 THEN f_user2_markname 
        ELSE f_user1_markname 
    END AS markname
FROM 
    t_user_relationship
WHERE 
    f_user_id1 = 1003 OR f_user_id2 = 1003;
--------------------------------------------------------------------
+-----------+--------------+----------+
| friend_id | teamname     | markname |
+-----------+--------------+----------+
|      1004 | 我的好友     | 刘倩     |
|      1001 | 我的好友     | Root     |
+-----------+--------------+----------+
2 rows in set (0.00 sec)
*/

struct FriendInfo {
    int friend_id;
    std::string teamname;
    std::string markname;
};

class BoostMysql{
    using tcp = boost::asio::ip::tcp;
    struct DatabaseInfo{
        std::string host;
        std::string port;
        std::string usr;
        std::string pass;
        std::string dbname;
    };
private:
    DatabaseInfo                     info_;
    boost::asio::ssl::context        sslIoc_;
    boost::mysql::tcp_ssl_connection conn_;
    boost::mysql::statement          stmt_;
    boost::mysql::results            result_;
    bool        bInit_;

    
public:
    BoostMysql();
    ~BoostMysql();
    BoostMysql(boost::asio::io_context&);
    bool Initialize(
        std::string&,std::string&,
        std::string&,std::string&,std::string&);
    bool CheckUserInfo(std::string& usr,std::string& pass);

    SqlResult ExecSql(const std::string& sql);
    std::vector<FriendInfo> GetFriends(int user_id);
    std::vector<int>        GetAllFriendIds(int userId);

    std::string             GetFriendImagePath(int friendId);
    
private:
    
    void disconnect();
};






