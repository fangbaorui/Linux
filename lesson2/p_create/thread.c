#include <stdio.h>
#include <unistd.h>  // ==> 系统调用接口需要包含的头文件
#include <pthread.h> // ==> 线程需要包含的头文件

void* MyThreadStart(void* arg)
{
  while(1)
  {
    printf("i am work thread\n");
    sleep(1);
  }
}

int main()
{
  pthread_t tid;
  int ret = pthread_create(&tid, NULL, MyThreadStart, NULL);
  if(ret < 0)
  {
    perror("pthread_create");
    return 0;
  }

  pthread_join(tid, NULL);
  while(1)
  {
    printf("i am main thread\n");
    sleep(1);
  }

  return 0;
}

