#include<stdio.h>
#include<iostream>
#include<unistd.h>
#include<pthread.h>
#include<queue>

using namespace std;

#define CAPACITY 10
#define THREADCOUNT 1

class RingQueue
{
  public:
    RingQueue()
    {
      capacity_=CAPACITY;

      pthread_mutex_init(&lock_,NULL);
      pthread_cond_init(&cons_cond_,NULL);
      pthread_cond_init(&prod_cond_,NULL);

    }

    ~RingQueue()
    {
      pthread_mutex_destroy(&lock_);
      pthread_cond_destroy(&cons_cond_);
      pthread_cond_destroy(&prod_cond_);

    }

    //Push->生产者线程调用函数
    void Push(int data)
    {
      pthread_mutex_lock(&lock_);
      while(que_.size() >= capacity_)
      {
         pthread_cond_wait(&prod_cond_,&lock_);
      }
      que_.push(data);
      pthread_mutex_unlock(&lock_);

      //通知消费者来进行消费
      pthread_cond_signal(&cons_cond_);
    }



    //Pop-》消费者线程调用函数
    void Pop(int* data)
    {
      pthread_mutex_lock(&lock_);
      while(que_.size() <= 0)
      {
        pthread_cond_wait(&cons_cond_,&lock_);
      }
      *data = que_.front();
      que_.pop();
      pthread_mutex_unlock(&lock_);
      //通知生产者来进行生产
      pthread_cond_signal(&prod_cond_);
    }



  private:
    queue<int> que_;
    size_t capacity_;

    //保证queue操作是互斥的
    pthread_mutex_t lock_;

    //消费者的条件变量
    pthread_cond_t cons_cond_;
    //生产者的条件变量
    pthread_cond_t prod_cond_;
};

void* ConsumeStart(void* arg)
{
   RingQueue* rq =(RingQueue*)arg;
   while(1)
   {
     int data;
     rq->Pop(&data);
     printf("i am ConsumeStart %p,i consume %d\n",pthread_self(),data);
   }
   return NULL;
}

int g_prodata = 0;

void* ProductStart(void* arg)
{
  RingQueue* rq = (RingQueue*)arg;
  while(1)
  {
    rq->Push(g_prodata++);
    sleep(1);
  }
  return NULL;
}

int main()
{
  RingQueue* rq = new RingQueue();
  if(rq == NULL)
  {
    printf("create ringqueue failed\n");
    return 0;
  }
  pthread_t cons[THREADCOUNT],prod[THREADCOUNT];
  for(int i = 0; i < THREADCOUNT; i++)
  {
    int ret = pthread_create(&cons[i],NULL,ConsumeStart,(void*)rq);
    if(ret < 0)
    {
      perror("pthread_create");
      return 0;
    }
    ret = pthread_create(&prod[i],NULL,ProductStart,(void*)rq);
    if(ret < 0)
    {
      perror("pthread_create");
      return 0;
    }
  }
  for(int i = 0; i < THREADCOUNT; i++)
  {
    pthread_join(cons[i],NULL);
    pthread_join(prod[i],NULL);
  }
  return 0;
}
