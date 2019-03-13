#pragma ocne

#include<vector>
#include<pthread.h>
#include<iostream>
#include<string>
#include"ProtocolUtil.hpp"
#include"Message.hpp"
#include"Window.hpp"

#define TCP_PORT 8080
#define UDP_PORT 8888


class ChatClient;

struct ParamPair{
    Window *wp;
    ChatClient *cp;
};



class ChatClient{
private:
    int tcp_sock;
    int udp_sock;
    std::string peer_ip;

    std::string passwd;
    
public:    
    std::string nick_name;
    std::string school;
    unsigned int id;
    
    struct sockaddr_in server;

public:
    ChatClient(std::string ip_):peer_ip(ip_)
    {
	id = 0;
	tcp_sock = -1;
	udp_sock = -1;
	server.sin_family = AF_INET;
	server.sin_port = htons(UDP_PORT);
	server.sin_addr.s_addr = inet_addr(peer_ip.c_str());
    }
    
    void InitClient()
    {
	udp_sock = SocketApi::Socket(SOCK_DGRAM);
    }

    bool ConnectServer()
    {
	tcp_sock = SocketApi::Socket(SOCK_STREAM);
	return SocketApi::Connect(tcp_sock, peer_ip, TCP_PORT);
    }

    bool Register()
    {
	if(Util::RegisterEnter(nick_name, school, passwd) && ConnectServer()){
	    Request rq;
	    rq.method = "REGISTER\n";

	    Json::Value root;
	    root["name"] = nick_name;
	    root["school"] = school;
	    root["passwd"] = passwd;


	    Util::Seralizer(root, rq.text);

	    rq.content_length = "Content_Length: ";
	    rq.content_length += Util::IntToString((rq.text).size());
	    rq.content_length += "\n";

	    Util::SendRequest(tcp_sock, rq);
	    recv(tcp_sock, &id, sizeof(id), 0);
	    
	    bool res = false;
	    if(id >= 10000){
		std::cout << "Register Success! Your Login ID Is :" << id << std::endl;
		res = true;
	    }else{
		std::cout << "Register Failed! Code is :" << id << std::endl;
	    }

	    close(tcp_sock);
	    return res;
	}
    }

    bool Login()
    {
	if(Util::LoginEnter(id, passwd) && ConnectServer()){
	    Request rq;
	    rq.method = "LOGIN\n";

	    Json::Value root;
	    root["id"] = id;
	    root["passwd"] = passwd;

	    Util::Seralizer(root, rq.text);


	    rq.content_length = "Content_Lengeh: ";
	    rq.content_length += Util::IntToString((rq.text).size());
	    rq.content_length += "\n";


	    Util::SendRequest(tcp_sock, rq);

	    
	    unsigned int result = 0;
	    recv(tcp_sock, &result, sizeof(result), 0);
	    bool res = false;
	    if(result >= 10000){
		res = true;
		std::string name_ = "None";
		std::string school_ = "None";
		std::string text_ = "Hello   I am login!!!";
		unsigned int id_ = result;
		unsigned int type_ = LOGIN_TYPE;

		Message m(name_, school_, text_, id_, type_);
		std::string sendString;
		m.ToSendString(sendString);
		UdpSend(sendString);


		std::cout << "Login Success! Your ID Is:" << id << std::endl;
	    }else{
		std::cout << "Login Failed! Code is :" << result << std::endl;
	    }

	    close(tcp_sock);
	    return res;
	}
    }
    void UdpSend(const std::string &message)
    {
	Util::SendMessage(udp_sock, message, server);
    }
    void UdpRecv(std::string &message)
    {
	struct sockaddr_in peer;
	Util::RecvMessage(udp_sock, message, peer);
    }


    static void *Welcome(void *arg)
    {
	pthread_detach(pthread_self());
	Window *wp = (Window*)arg;
	wp->Welcome();
    }
    static void *Input(void *arg)
    {
	
	pthread_detach(pthread_self());
	struct ParamPair *pptr = (struct ParamPair*)arg;
	Window *wp = pptr->wp;
	ChatClient *cp = pptr->cp;

	wp->DrawInput();
	std::string text;

	for(;;){
	    wp->GetStringFromInput(text);
	    
	    Message msg(cp->nick_name, cp->school, text, cp->id);
	    std::string sendString;
	    msg.ToSendString(sendString);
	    
	    cp->UdpSend(sendString);
	}
    }

    void Chat()
    {
	Window w;
	pthread_t h, l;
    
	struct ParamPair pp = {&w, this};

	pthread_create(&h, NULL, Welcome, &w);
	pthread_create(&l, NULL, Input, &pp);
	
	std::string recvString;
	std::string showString;
	std::vector<std::string> online;

	w.DrawOutput();
	w.DrawOnline();
	for(;;){
	    UdpRecv(recvString);
	    Message msg;
	    msg.ToRecvValue(recvString);
	    
	    if(msg.Id() == id && msg.Type() == LOGIN_TYPE){
		nick_name = msg.NickName();
		school = msg.School();
	    }
	    
	    showString = msg.NickName();
	    showString += " - ";
	    showString += msg.School();
	    
	    
	    std::string f = showString;//zhangsan-qinghua
	    Util::addUser(online, f);
	    
	    
	    showString += " # ";
	    showString += msg.Text();//zhangsan-sust# nihao
	    w.PutMessageToOutput(showString);

	    w.PutUserToOnline(online);
	}
    }



    void Logout()
    {
    //1.Message
    //2.type logout
    //3.clientserver
    }
    
    ~ChatClient()
    {
    }

};
