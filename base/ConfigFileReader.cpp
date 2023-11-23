#include "ConfigFileReader.hpp"

ConfigFileReader::ConfigFileReader(const std::string& filename){
    LoadFile(filename);
}

ConfigFileReader::~ConfigFileReader(){}


//根据键值（key）对名称返回对应值（value）
std::string ConfigFileReader::GetConfigValue(const std::string& name){
    if(!this->loadOK_)
        return nullptr;
    std::string val;
    std::map<std::string,std::string>::iterator it = configMap_.find(name);
    if(it != configMap_.end()){ //如果没有轮到end()，则代表找到，获取键值
        val = it->second;
    }

    return val;     //  返回临时的string类型并不需要手动new堆上

}

//////////////////////////////////////////////////////////////////////
//读，分析，处理操作
/*
    读取文件
    并一行行预处理（删除注释，删除行尾换行符，删除空行）
*/
void ConfigFileReader::LoadFile(const std::string& filename){
    configFile_.clear();
    configFile_ = filename;

    std::ifstream ifs(filename);
    if(!ifs.is_open())    return;
    std::string line;
    while (std::getline(ifs, line))
    {
        // 删除注释
        size_t commentPos = line.find('#');
        if (commentPos != std::string::npos)
            line = line.substr(0, commentPos);  //保存该行从0到#之前的数据

        // 删除行尾换行符
        if (!line.empty() && line.back() == '\n')
            line.pop_back();

        // 如果行为空，则继续
        if (line.empty())
            continue;

        ParseLine(line);    //解析该行数据
    }

    ifs.close();
    loadOK_ = true;     //只有载入成功才会设置true。
                        //也只有它true，才能后续set,get
}

/*
    解析该行    并更新该对象map<string,string>数据
    分为键（key），值（value）部分数据
    并在解析中调用TrimSpace函数分别处理key，val的空格和制表符
*/
void ConfigFileReader::ParseLine(std::string& line){
    std::size_t equalPos = line.find('=');
    if(equalPos == std::string::npos)
        return;
    std::string key = line.substr(0,equalPos);
    std::string val = line.substr(equalPos+1);

    this->TrimSpace(key);this->TrimSpace(val);

    if(!key.empty() && !val.empty())
        this->configMap_[key] = val;



}

//处理传入数据中的空格和制表符\t
void ConfigFileReader::TrimSpace(std::string& str){
    //注意，此处find是寻找空格或\t或\r，并不是连续才能被检测到
    std::size_t start = str.find_first_not_of(" \t\r");
    std::size_t end   = str.find_last_not_of(" \t\r");

    if(start != std::string::npos && end != std::string::npos){
        str = str.substr(start,end - start + 1);//substr第二个参数为长度
    }else if(start != std::string::npos)
        str = str.substr(start);
    else
        str.clear();
    
}

//////////////////////////////////////////////////////////////////////
//写操作
/*
    只是将成员变量configMap_的值写入配置文件中。
    而configMap_值的设定需要在函数SetConfigValue中设定
*/
int ConfigFileReader::WriteFile(const std::string& filename){
    std::string filepath = filename.empty() ? configFile_ : filename;
    std::ofstream ofs(filepath);    //ofstream，不是ostream

    if(!ofs.is_open())
        return -1;
    
    for(const auto& pair : configMap_){
        ofs<<pair.first << "=" <<
             pair.second<< "\n";
        if(!ofs)
            return -1;
    }
    return 0;

}

/*
    设置configmap的值，并据此在writefile中更新配置文件信息
*/
int ConfigFileReader::SetConfigValue(const std::string& key,const std::string& value){
    if(!this->loadOK_)
        return -1;
    //更新map
    this->configMap_[key] = value;
    //根据更新后的map更改写入配置文件
    return this->WriteFile();
}


