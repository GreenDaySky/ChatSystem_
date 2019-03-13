#pragma once

#include<iostream>
#include<string>
#include<vector>
//信号量
#include<semaphore.h>

class DataPool{
private:
    std::vector<std::string> pool;
    int cap;
    sem_t data_sem;
    sem_t blank_sem;
    int product_step;
    int consume_step;

public:
    DataPool(int cap_ = 512):cap(cap_) ,pool(cap_)
    {
	sem_init(&data_sem, 0, 0);//初始化为0
	sem_init(&blank_sem, 0, cap);//初始化为cap
	product_step = 0;
	consume_step = 0;
    }

    void PutMessage(const std::string &msg)
    {
	sem_wait(&blank_sem);
	pool[product_step] = msg;
	product_step++;
	product_step %= cap;
	sem_post(&data_sem);
    }
    void GetMessage(std::string &msg)
    {
	sem_wait(&data_sem);
	msg = pool[consume_step];
	consume_step++;
	consume_step %= cap;
	sem_post(&blank_sem);
    }

    ~DataPool()
    {
	sem_destroy(&data_sem);
	sem_destroy(&blank_sem);
    }

};
