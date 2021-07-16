#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>

void func(){
  //exit(1); 
  _exit(99);
    }

int main()
{
   int pid = fork();
   if(pid < 0)
   {
     perror("fork error");
   }
   perror("fork error");



   printf("----------"); //没有换行，不会刷新缓冲区，程序退出才会刷新缓冲区
   sleep(3);

   func();

   //return 0;
   printf("我已经退出了\n");  //不会被打印
}
