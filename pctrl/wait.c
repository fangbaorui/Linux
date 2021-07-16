#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>  //wait
#include<sys/wait.h>

int main()
{
  pid_t pid = fork();
  if(pid < 0)
  {
     perror("fork error");
     return -1;
  }
  else if(pid == 0)
  {
    //child
    sleep(5);
    return 99;//子进程退出
  }
  int ret = wait(NULL);  //进程等待，如果不等待，子进程成为僵尸进程
  printf("progress:%d--%d exit\n",ret,pid); //5秒后，子进程退出后才会打印
  while(1)
  {
    sleep(1);
  }
  return 0;
}
