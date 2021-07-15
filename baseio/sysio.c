#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<fcntl.h>
#include<string.h>
#include<sys/stat.h>

int main()
{
  umask(0);//将当前进程的文件权限掩码设置为0
  //int open(文件名，选项标志，权限)
  int fd = open("test.txt", O_RDWR|O_CREAT|O_CREAT, 0777);
  if(fd < 0)
  {
    perror("open error");
    return -1;
  }

  char *ptr = "昏昏余数--\n";
  int ret = write(fd, ptr, strlen(ptr));
  if( ret < 0 )
  {
    perror("fwriter error");
    return -1;
  }

  //lseek(句柄，偏移量，相对位置)
  lseek(fd, 0, SEEK_SET);

  char buf[1024] = {0};
  ret = read(fd, buf, 1023);
  if(ret < 0)
  {
    perror("read error");
    return -1;
  }
  printf("%d--%s\n",ret, buf);
  close(fd);
  return 0;
}
