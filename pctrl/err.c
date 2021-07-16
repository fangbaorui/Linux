#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>

int main()
{
  for(int i = 0; i <= 255; i++)
  {
    printf("%d------%s\n",i ,strerror(i));
  }
  return 0;
}
