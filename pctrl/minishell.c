#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<ctype.h>
#include<fcntl.h>
#include<sys/stat.h>
int main(int agrc, char* argv[])
{
  while(1)
  {
    printf("[urse@host path]$ ");
    fflush(stdout);

    char cmd[1024] = {0};
    fgets(cmd, 1023, stdin);
    cmd[strlen(cmd) - 1] = '\0';

    int direct_flag = 0;// 0-没有 1-清空 2-追加
    char* ptr = cmd;

    char* redirect_file = NULL;

    //[ ls  -a  \0\0  text.txt ]
    while(*ptr != '\0'){
      if(*ptr == '>')
      {
        direct_flag = 1;
        *ptr = '\0';
        ptr++;
        if(*ptr == '>'){
          direct_flag = 2;
          *ptr = '\0';
          ptr++;
        }
        while(*ptr != '\0' && *ptr == ' ')ptr++;
        redirect_file = ptr;
        while(*ptr != '\0' && *ptr != ' ')ptr++;
        *ptr = '\0';
      }
      ptr++;
    }


    ptr = cmd;
    char* arg[32] = {NULL};
    int agc = 0;

    while(*ptr != '\0')
    {
      if(!isspace(*ptr))
      {
        arg[agc] = ptr;
        agc++;
        while(*ptr != '\0' && !isspace(*ptr))
          ptr++;
        *ptr = '\0';
      }
      ptr++;
    }
    arg[agc] = NULL;
    /*for(int i = 0; i < agc; i++)
      {
      printf("[%s]\n",arg[i]);
      }*/

    if(strcmp(arg[0], "cd") == 0){
      chdir(arg[1]);
      continue;
    }

    pid_t pid = fork();
    if(pid < 0)
    {
      continue;
    }else if(pid == 0)
    {
      if(direct_flag == 1)
      {
        int fd = open(redirect_file,O_CREAT|O_RDWR|O_TRUNC, 0664);
        dup2(fd,1);
      }
      else if(direct_flag == 2)
      {
        int fd = open(redirect_file,O_CREAT|O_RDWR|O_APPEND,0664);
        dup2(fd,2);
      }
      execvp(arg[0], arg);
      exit(-1);
    }
    wait(NULL);
  }
  return 0;
}
