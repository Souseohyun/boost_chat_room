/*
    配置文件读取类
    kazuki.cschn@gmail.com
*/

#pragma once

#include<map>
#include<string>
#include<memory>
#include<fstream>
#include<iostream>

class ConfigFileReader{
private:
    bool loadOK_;   //配置文件是否成功加载
    std::map<std::string,std::string> configMap_;//储存配置文件键值对
    std::string configFile_;    //配置文件路径

    //void    LoadFile(const char* filename);
    void    LoadFile(const std::string& filename);
    int     WriteFile(const std::string& filename = nullptr);
    void    ParseLine(std::string& line);
    void   TrimSpace(std::string& name);

public:
    //ConfigFileReader(const char* filename);
    ConfigFileReader(const std::string& filename);
	~ConfigFileReader();

    //char* GetConfigName(const char* name);
    std::string GetConfigValue(const std::string& name);
    int SetConfigValue(const std::string& name, const std::string&  value);

};