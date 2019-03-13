#pragma once

#include<iostream>
#include"ProtocolUtil.hpp"
#include<pthread.h>
#include"UserManager.hpp"
#include"log.hpp"
#include"DataPool.hpp"
#include"Message.hpp"


class ChatServer;

class Param{
public:
    ChatServer* sp;
    int sock;

    std::string ip;
    int port;

    Param(ChatServer* sp_, int &sock_, const std::string &ip_,const int &port_):sp(sp_), sock(sock_), ip(ip_), port(port_)
    {
    
    }
    ~Param(){}
};

class ChatServer{
private:
    int tcp_port;
    int tcp_listen_sock;

    int udp_work_sock;
    int udp_port;
    UserManager um;
    DataPool pool;

public:
    ChatServer(int tcp_port_ = 8080, int udp_port_ = 8888):
	tcp_port(tcp_port_),
	tcp_listen_sock(-1),
	udp_port(udp_port_),
	udp_work_sock(-1)
	{}
    
    void InitServer()
    {
	tcp_listen_sock = SocketApi::Socket(SOCK_STREAM);
	udp_work_sock = SocketApi::Socket(SOCK_DGRAM);
	SocketApi::Bind(tcp_listen_sock, tcp_port);
	SocketApi::Bind(udp_work_sock, udp_port);

	SocketApi::Listen(tcp_listen_sock);
    }

    unsigned int RegisterUser(const std::string &name, const std::string &school, const std::string &passwd)
    {
	return um.Insert(name, school, passwd);
    }
    unsigned int LoginUser(const unsigned int &id, const std::string &passwd, const std::string &ip, const int &port)
    {
	unsigned int result = um.Check(id, passwd);
	return um.Check(id, passwd);
    }


    //UDP
    void Product()
    {
	std::string message;
	struct sockaddr_in peer;
	Util::RecvMessage(udp_work_sock, message, peer);
	std::cout << "debug: recv message: " << message << std::endl;
	if(!message.empty()){ 
	    Message m;
	    m.ToRecvValue(message);
	    if(m.Type() == LOGIN_TYPE){
		um.AddOnlineUser(m.Id(), peer);
		std::string name_; 
		std::string school_;
		um.GetUserInfo(m.Id(), name_, school_);
		Message new_msg(name_, school_, m.Text(), m.Id(), m.Type());
		new_msg.ToSendString(message);
	    }
	    
	    
	    pool.PutMessage(message);
	}

    }
    void Consume()
    {
	std::string message;
	pool.GetMessage(message);
	std::cout << "debug: send message: " << message << std::endl;
	auto online = um.OnlineUser();
	for(auto it = online.begin(); it != online.end(); it++){
	    Util::SendMessage(udp_work_sock, message, it->second);
	}
    }



//pthread_craete第三个参数是一个void*的函数指针，但此处是一个类成员函数，为了不使它带有this指针
//加上关键字static
    static void *HandlerRequest(void *arg)
    {
	Param* p = (Param*)arg;
	int sock = p->sock;
	ChatServer* sp = p->sp;
	int port = p->port;
	std::string ip = p->ip;

	delete p;
	pthread_detach(pthread_self());
	
	Request rq;
	Util::RecvRequest(sock, rq);

	//std::cout << rq.text << std::endl;
	//rq.text为NULL
	
	Json::Value root;
	LOG(rq.text, NORMAL);
	Util::UnSeralizer(rq.text, root);

    
	if(rq.method == "REGISTER"){
	    std::string name = root["name"].asString();//asInt
	    std::string school = root["school"].asString();
	    std::string passwd = root["passwd"].asString();


	    //std::cout << root["name"] << std::endl;
	    //std::cout << school << std::endl;
	    //std::cout << passwd << std::endl;

	    unsigned int id = sp->RegisterUser(name, school, passwd);
	    send(sock, &id, sizeof(id), 0);
	}else if(rq.method == "LOGIN"){
	    unsigned int id = root["id"].asInt();
	    std::string passwd = root["passwd"].asString();
	    
	    //std::cout << passwd << std::endl;
	    //std::cout << "hello"  << std::endl;

	    //check, get to online
	    unsigned int result = sp->LoginUser(id, passwd, ip, port);

	    send(sock, &result, sizeof(result), 0);
	}else{
	    //Logout
	} 
	//recv:sock
	//分析&&处理
	//response
    close(sock);
    }
    void Start()
    {
	int port;
	std::string ip;
	for(;;){
	    int sock = SocketApi::Accept(tcp_listen_sock, ip, port);
	    if(sock > 0){
		std::cout << "get a new client:" << ip << ":" << port << std::endl;
		
		Param* p = new Param(this, sock, ip, port);
		pthread_t tid;
		pthread_create(&tid, NULL, HandlerRequest, p);
		
	    }
	}
    }
    ~ChatServer()
    {}
};








