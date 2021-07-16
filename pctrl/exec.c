#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>

int main(int argc,char* argv[])
{
  printf("我要进行程序替换\n");
  //execl("/usr/bin/ls","ls","-a","-l",NULL);
  char* arg[] = {"ls","-a","-l",NULL};
  //execv("/usr/bin/ls",arg);
  //execvp("ls",arg);
  char* env[] = {"MYVAL = 1000", NULL};
  //execve("./env", arg, env);
  extern char** environ;
  //execve("./env", arg, environ);
  execle("./env", "env", "-a", "-l", NULL, env);
  
  printf("程序替换完毕了\n");//程序替换成功不会打印这句话
  return 0;
}
