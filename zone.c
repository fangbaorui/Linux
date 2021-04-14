#include<stdio.h>
#include<unistd.h>

int g_val=100;
int main(int argc,char* argv[])
{
  pid_t pid = fork();
  if(pid < 0)
  {
    return -1; // 创建子进程失败
  }
  else if(pid==0)
  {
    //子进程会返回0
    g_val=200;
    printf("child g_val=%d--%p\n",g_val,&g_val);
  }
  else{
    //父进程返回子进程的pid是大于0的
    sleep(3);
    printf("parent g_val=%d--%p\n",g_val,&g_val);
  }
  return 0;
}
