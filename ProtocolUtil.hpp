#pragma once

#include<vector>
#include<iostream>
#include"log.hpp"
#include<sys/socket.h>
#include<sstream>
#include<sys/types.h>
#include<string>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<unistd.h>
#include<stdlib.h>
#include"json/json.h"


#define BACKLOG 5
#define MESSAGE_SIZE 512

class Request{
public:
    std::string method;//Login Register Logout
    std::string content_length;//"Content_Legth:89"
    std::string blank;
    std::string text;
public:
    Request():blank("\n")
    {}
    ~Request()
    {}
};

class Util{
public:
    static bool RegisterEnter(std::string &n_, std::string &s_, std::string &passwd)
    {
        std::cout << "Please Enter Nick Name:";
        std::cin >> n_;
        std::cout << "Please Enter School:";
        std::cin >> s_;
        std::cout << "Please Enter Passwd:";
        std::cin >> passwd;
        std::string again;
        std::cout << "Please Enter Passwd Again:";
        std::cin >> again;
        if(passwd == again){
	    return true;
        }else{
	    std::cout << "Your Passwd wrong!" <<std::endl;
	    return false;
	}
    }

    static bool LoginEnter(unsigned int &id, std::string &passwd)
    {
	std::cout << "Please Enter ID:";
	std::cin >> id;
	std::cout << "Please Enter Your Passwd:";
	std::cin >> passwd;
	return true;
	/*if(id > 10000){
	    return true;
	}else{
	    return false;
	}*/
    }
    static void Seralizer(Json::Value &root, std::string &outString)
    {
	Json::FastWriter w;
	outString = w.write(root);
    }
    static void UnSeralizer(std::string &inString, Json::Value &root)
    {
	Json::Reader r;
	r.parse(inString, root, false);
    }

    static std::string IntToString(int x)
    {
	std::stringstream ss;
	ss << x;
	return ss.str();
    }

    static int StringToInt(std::string &str)
    {
	int x;
	std::stringstream ss(str);
	ss >> x;
	return x;
    }

    static void RecvOneLine(int sock, std::string &outString)
    {
	char c = 'x';
	while(c != '\n'){
	    ssize_t s = recv(sock, &c, 1, 0);
	    if(s > 0){
		if(c == '\n'){
		    break;
		}
		outString.push_back(c);
	    }
	    else{
		break;
	    }
	}

    }

    //TCP
    static void RecvRequest(int sock, Request &rq)
    {
	RecvOneLine(sock, rq.method);
	RecvOneLine(sock, rq.content_length);
	RecvOneLine(sock, rq.blank);

    
	std::string &cl = rq.content_length;//content_length:88
	std::size_t pos = cl.find(": ");
	if(std::string::npos == pos){
	    std::cout << "llllllllllll" << std::endl;
	    return;
	}
	std::string sub = cl.substr(pos+2);
	int size = StringToInt(sub);
	char c;
	for(auto i = 0; i < size; i++){
	    recv(sock, &c, 1, 0);
	    (rq.text).push_back(c);
	}

    }
    static void SendRequest(int sock, Request &rq)
    {
	std::string &m_ = rq.method;
	std::string &cl_ = rq.content_length;
	std::string &b_ = rq.blank;
	std::string &text_ = rq.text;
	send(sock, m_.c_str(), m_.size(), 0);
	send(sock, cl_.c_str(), cl_.size(), 0);
	send(sock, b_.c_str(), b_.size(), 0);
	send(sock, text_.c_str(), text_.size(), 0);
    }

    //UDP
    static void RecvMessage(int sock, std::string &message, struct sockaddr_in &peer)
    {
	char msg[MESSAGE_SIZE];
	socklen_t len = sizeof(peer);
	ssize_t s = recvfrom(sock, msg, sizeof(msg)-1, 0, (struct sockaddr*)&peer, &len);
	if(s < 0){
	    LOG("recvfrom message error", WARNING);
	}
	else{
	    message = msg;
	}
    }

    static void SendMessage(int sock, const std::string &message, struct sockaddr_in &peer)
    {
	sendto(sock, message.c_str(), message.size(), 0, (struct sockaddr*)&peer, sizeof(peer));
    }

    static void addUser(std::vector<std::string> &online, std::string &f)
    {
	for(auto it = online.begin(); it != online.end(); it++)
	{
	    if(*it == f){
		return;
	    }
	}
	online.push_back(f);
    }

};


class SocketApi{
public:
    static int Socket(int type)
    {
	int sock = socket(AF_INET, type, 0);
	if(sock < 0){
	    LOG("sock error!", ERROR);
	    exit(2);
	}
	return sock;
    }

    static void Bind(int sock, int port)
    {
	struct sockaddr_in local;
	local.sin_family = AF_INET;
	local.sin_addr.s_addr = htonl(INADDR_ANY);
	local.sin_port = htons(port);

	if(bind(sock, (struct sockaddr*)&local, sizeof(local)) < 0){
	    LOG("socket error", ERROR);
	    exit(3);
	}
    }
    static void Listen(int sock)
    {
	if(listen(sock, BACKLOG) < 0){
	    LOG("listen error", ERROR);
	    exit(4);
	}
    }

    static int Accept(int listen_sock, std::string &out_ip, int &out_port)
    {
	struct sockaddr_in peer;
	socklen_t len = sizeof(peer);
	int sock = accept(listen_sock, (struct sockaddr*)&peer, &len);
	if(sock < 0){
	    LOG("accept error", WARNING);
	    return -1;
	}
	out_ip = inet_ntoa(peer.sin_addr);
	out_port = htons(peer.sin_port);

	return sock;
    }

    static bool Connect(const int sock, std::string peer_ip, const int port)
    {
	struct sockaddr_in peer;
	peer.sin_family = AF_INET;
	peer.sin_addr.s_addr = inet_addr(peer_ip.c_str());
	peer.sin_port = htons(port);

	if(connect(sock, (struct sockaddr*)&peer, sizeof(peer)) < 0){
	    LOG("connect error", WARNING);
	    return false;
	}

	return true;
    }


};













