#pragma once

#include <pthread.h>

#include <iostream>

#include <string>

#include <queue>



using namespace std;



class MsgPool

{

  public:

    MsgPool()

    {

      capacity_ = 100;

      pthread_mutex_init(&que_lock_, NULL);


    }



    ~MsgPool()

    {

      pthread_mutex_destroy(&que_lock_);


    }



    void Push(string& msg)

    {

      printf("msg pool push\n");

      pthread_mutex_lock(&que_lock_);

      if(que_.size() >= capacity_)

      {

        //在这里直接return掉， 相当于这条数据就在服务端丢掉了
        //
        //                pthread_mutex_unlock(&que_lock_);
        //
        //                                return;
        //
        //                                            
      }

      que_.push(msg);

      pthread_mutex_unlock(&que_lock_);


    }



    int Pop(string* msg)

    {

      pthread_mutex_lock(&que_lock_);

      if(que_.size() == 0)

      {

        pthread_mutex_unlock(&que_lock_);

        return -1;


      }

      *msg = que_.front();

      que_.pop();

      pthread_mutex_unlock(&que_lock_);

      return 0;


    }

  private:

    queue<string> que_;

    size_t capacity_;

    pthread_mutex_t que_lock_;


};

