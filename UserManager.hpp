#pragma once

#include<iostream>
#include<string>
#include<unordered_map>
#include<pthread.h>

class User{
private:
    std::string nick_name;
    std::string school;
    std::string passwd;
public:
    //error user[i]
    User()
    {
    }

    User(const std::string &n_, const std::string &s_,\
	const std::string pwd_):
	nick_name(n_), school(s_), passwd(pwd_)
	{

	}
	bool IsPasswdOK(const std::string &passwd_)
	{
	    return passwd == passwd_ ? true:false;
	}

	std::string &GetNickName()
	{
	    return nick_name;
	}
	std::string &GetSchool()
	{
	    return school;
	}
    ~User()
    {}
};

class UserManager{
private:
    unsigned int assign_id;
    std::unordered_map<unsigned int, User>users;
    std::unordered_map<unsigned int, struct sockaddr_in> online_users;
    pthread_mutex_t lock;
    
    void Lock()
    {
	pthread_mutex_lock(&lock);
    }
    void UnLock()
    {
	pthread_mutex_unlock(&lock);
    }

public:
    UserManager():assign_id(10000)
    { 
	pthread_mutex_init(&lock, NULL);
    }
    unsigned int Insert(const std::string &n_, const std::string &s_, const std::string &p_)
    {
	Lock();
	unsigned int id = assign_id++;
	User u(n_, s_, p_);
	if(users.find(id) == users.end()){
	   //user.insert(make_pair(id, u));
	   users.insert({id, u});
	   UnLock();
	   return id;
	}
	
	UnLock();
	return 1;
    }
    unsigned int Check(const unsigned int &id, const std::string &passwd)
    {
	Lock();
	auto it = users.find(id);
	if(it != users.end()){
	    User &u = it->second;
	   if(u.IsPasswdOK(passwd)){
	    UnLock();
	    return id;
	   }
	}
	UnLock();
	return 2;
    
    }

    void AddOnlineUser(unsigned int id, struct sockaddr_in &peer)
    {
	Lock();
	auto it = online_users.find(id);
	if(it == online_users.end()){
	    online_users.insert({id, peer});
	}
	UnLock();
    }

    void GetUserInfo(const unsigned int &id, std::string &name_,\
	std::string &school_)
	{
	    Lock();
	    
	    name_ = users[id].GetNickName();
	    school_ = users[id].GetSchool();

	    UnLock();
	}
    std::unordered_map<unsigned int, struct sockaddr_in> OnlineUser()
    {
	Lock();
	auto online = online_users;
	UnLock();
	return online;
    }

    ~UserManager()
    {
	pthread_mutex_destroy(&lock);
    }
};


























