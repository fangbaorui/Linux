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
  int ret,status;
  while((ret = waitpid(-1,&status,WNOHANG)) == 0)  //进程等待，如果不等待，子进程成为僵尸进    程
  {
    printf("当前子进程还没有退出\n");
    sleep(1);
  }
    if( ret < 0 )
    {
      perror("waitpid error");
    }
    
  if((status & 0x7f) != 0)
  {
    printf("进程异常退出\n");
    return -1;
  }
  int retval = (status >>8) & 0xff;

  printf("progress:%d--%d  exit -- retval\n",ret,pid,retval); //5秒后，子进程退出后    才会打印                                                         
  while(1)
  {
    sleep(1);

  }
  return 0;

}

