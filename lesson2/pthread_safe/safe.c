#include <stdio.h>
#include <pthread.h>
#include<unistd.h>
#define THREADCOUNT 2

int g_tickets = 100;
pthread_mutex_t g_lock;

void* MyThreadStart(void* arg)
{
  while(1)
  {
    pthread_mutex_lock(&g_lock);
    if(g_tickets > 0)
    {
      printf("i am %p, i have %d ticket\n", pthread_self(),g_tickets--);
    }
    else
    {
      pthread_mutex_unlock(&g_lock);
      pthread_exit(NULL);
    }
    pthread_mutex_unlock(&g_lock);
  }
  return NULL;
}

int main()
{
  pthread_mutex_init(&g_lock, NULL);
  pthread_t tid[THREADCOUNT];
  for(int i = 0; i < THREADCOUNT; i++)
  {
    int ret = pthread_create(&tid[i], NULL, MyThreadStart, NULL);
    if(ret < 0)
    {
      perror("pthread_create");
      return 0;
    }
  }

  for(int i = 0; i < THREADCOUNT; i++)
  {
    pthread_join(tid[i], NULL);
  }
  pthread_mutex_destroy(&g_lock);
  return 0;
}



