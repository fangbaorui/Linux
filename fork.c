#include<stdio.h>
#include<stdlib.h>
#include<unistd.h> 
int main(int argc,char* argv[])
{
     printf("-------------\n");
     pid_t pid = fork();
     if(pid < 0)
     {
       printf("出错了\n"); //子进程赋值了父进程，因此往后的代码父子进程都会运行，但是因为返回值不同因此进入不同的if
     }
     else if(pid == 0)
     {
       printf("子进程\n"); //对于子进程返回值是0
       sleep(5);
       exit(0);  //退出进程，谁调用就推出谁
     }
     else{
       printf("父进程\n");// 对于父进程返回值是子进程pid，因此返回值是大于0的
     }
     printf("我是一个大帅哥:%d\n",getpid());
     while(1)
       sleep(1);
     return 0;
}

