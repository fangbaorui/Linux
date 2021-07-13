#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

int main()
{
  /*
   *1.create socket
   2.bind
   3.listen
   4.accept
   5.send/recv
   6.close
   */

  int listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if(listenfd < 0)
  {
    perror("socket");
    return 0;
  }

  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(17878);
  addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  
  int ret = bind(listenfd,(struct sockaddr*)&addr, sizeof(addr));
  if(ret < 0)
  {
    perror("bind");
    return 0;
  }

  ret = listen(listenfd,5);
  if(ret < 0)
  {
    perror("listen");
    return 0;
  }

  //成功返回新连接对应的套接字描述符
  int newsockfd = accept(listenfd, NULL, NULL);
  if(newsockfd < 0)
  {
    perror("accept");
    return 0;
  }

  while(1)
  {
    char buf[1024] = {0};
    ssize_t recv_size = recv(newsockfd, buf, sizeof(buf)-1,0);
    if(recv_size < 0)
    {
      perror("recvfrom");
      continue;
    }
    else if(recv_size == 0)
    {
      printf("peer shutdown!\n");
      close(newsockfd);
    }

    printf("client say:%s\n",buf);
    memset(buf,'\0',1024);

    printf("response client msg:");
    fflush(stdout);
    scanf("%s",buf);

    send(newsockfd, buf, strlen(buf),0);
  }

  close(listenfd);
  return 0;
}
