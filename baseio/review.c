#include<stdio.h>
#include<string.h>

int main(int argc,char* argv[])
{
  //FILE* fopen(文件名，打开方式)
  FILE* fp = fopen("test.txt", "wb+");
  if(fp == NULL)
  {
    perror("fopen error\n");
    return -1;
  }
  
  //fwrite(数据，块大小，块个数，句柄)
  char* ptr = "今天天气还不错\n";
  int ret = fwrite(ptr, strlen(ptr), 1, fp);
  if(ret == 0)
  {
    perror("fwrite error\n");
    return -1;
  }
  
  //fseek(句柄，偏移量，偏移相对起始位置)
  fseek(fp, 0, SEEK_SET);


  char buf[1024] = {0};
  ret = fread(buf, 1, 100, fp);
  if(ret == 0)
  {
    if(feof(fp))
    {
      printf("已经到达文件末尾\n");
    }
    if(ferror(fp))
    {
      perror("读取出错\n");
    }
  }
  printf("%d----%s\n",ret, buf);

  fclose(fp);
  return 0;

}
