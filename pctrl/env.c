#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>

int main(int argc, char* argv[])
{
   for(int i = 0; argv[i] != NULL; i++)
   {
     printf("argv[%d] = %s\n",i,argv[i]);
   }
   extern char** environ;
   for(int i = 0; environ[i] != NULL; i++)
   {
     printf("environ[%d] = %s\n",i,environ[i]);
   }
   return 0;
}
