#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

//main函数有三个参数-程序运行参数的个数，程序运行参数字符串地址，环境变量的字符串地址
int main(int argc,char *argv[],char* environ[])
{
 // extern char** environ;//声明已有的全局变量，这个变量中保存了所有能够访问到的环境变量的字符串地址

  int i;
  for(i=0; argv[i]!=NULL;i++){
    printf("argv[%d]=%s\n",i,argv[i]);
  }
  
  for(i-0;environ[i]!=NULL;i++)
  {
  //  printf("%s\n",environ[i]);
  }
  return 0;
}


/*
int main()
{
  char *env1=getenv("MYVAL1");
  if(env1==NULL)
  {
    printf("not fount MYVAL1\n");
  }
  else{
    printf("%s\n",env1);
  }
  char *env2=getenv("MYVAL2");
  if(env2==NULL)
    {
         printf("not fount MYVAL2\n");   
    }
    else{
        printf("%s\n",env2);
     }
    while(1)
      sleep(1);
    return 0;
}*/
