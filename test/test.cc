#include<iostream>
#include"json/json.h"
#include<string>

int main()
{
    std::string nick_name = "zhangsan";
    std::string school = "SUST";
    std::string passwd = "123456";

    Json::Value root;

    root["name"] = nick_name;
    root["school"] = school;
    root["passwd"] = passwd;

    Json::FastWriter w;
    std::string s = w.write(root);

    std::cout << s << std::endl;
   

    Json::Reader r;
    Json::Value client;
    if(r.parse(s, client)){
    std::cout << client["name"] << std::endl;
    std::cout << client["school"] << std::endl;
    std::cout << client["passwd"] << std::endl;
	return true;
    }else{
	std::cout << "false" << std::endl;
	return false;
    }

}
