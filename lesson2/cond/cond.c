#include<stdio.h>
#include<unistd.h>
#include<pthread.h>

#define THREADCOUNT 2

int g_bowl = 0;
pthread_mutex_t g_lock;

//测试只有一个变量的时候
//eat cond
pthread_cond_t g_cond;
//make cond
pthread_cond_t g_makecond;


void* EatThreadStart(void *arg)
{
  (void)arg;
  while(1)
  {
    pthread_mutex_lock(&g_lock);

    while(g_bowl <= 0)
    {
      pthread_cond_wait(&g_cond,&g_lock);
    }
    printf("i am %p,i eat %d\n",pthread_self(),--g_bowl);
    pthread_mutex_unlock(&g_lock);
    pthread_cond_signal(&g_makecond);
  }
  return NULL;
}

void* MakeThreadStart(void* arg)
{
  (void)arg;
  while(1)
  {
    pthread_mutex_lock(&g_lock);

    while(g_bowl > 0)
    {
      pthread_cond_wait(&g_makecond,&g_lock);
    }
    printf("i am %p, i maek %d\n",pthread_self(),++g_bowl);
    pthread_mutex_unlock(&g_lock);
    pthread_cond_signal(&g_cond);
  }
  return NULL;
}

int main()
{
  pthread_mutex_init(&g_lock,NULL);
  pthread_cond_init(&g_cond,NULL);
  pthread_cond_init(&g_makecond,NULL);

  pthread_t EatThread[THREADCOUNT],MakeThread[THREADCOUNT];
  for(int i = 0;i < THREADCOUNT;i++)
  {
    int ret=pthread_create(&EatThread[i],NULL,EatThreadStart,NULL);
    if(ret<0)
    {
      perror("pthread_create");
      return 0;
    }

    ret = pthread_create(&MakeThread[i],NULL,MakeThreadStart,NULL);
    if(ret<0)
    {
      perror("pthread_create");
      return 0;
    }
  }

  for(int i = 0;i < THREADCOUNT;i++)
  {
    pthread_join(EatThread[i],NULL);
    pthread_join(MakeThread[i],NULL);
  }

  pthread_mutex_destroy(&g_lock);
  pthread_cond_destroy(&g_cond);
  pthread_cond_destroy(&g_makecond);

  return 0;
}

