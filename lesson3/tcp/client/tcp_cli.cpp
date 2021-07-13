#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

int main()
{
  /*
   * 1.create socket
   * 2.bind
   * 3.listen
   * 4.accept
   * 5.send/recv
   * 6.close
   * */

  int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if(sockfd < 0)
  {
    perror("socket");
    return 0;
  }

  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(17878);
  addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  int ret = connect(sockfd, (struct sockaddr*)&addr, sizeof(addr));
  if(ret < 0)
  {
    perror("connect");
    return 0;
  }

  while(1)
  {
    char buf[1024] = {0};
    printf("please enter msg:");
    fflush(stdout);
    scanf("%s",buf);
    
    send(sockfd, buf, strlen(buf), 0);

    memset(buf,'\0',sizeof(buf));

    ssize_t recv_size = recv(sockfd, buf, sizeof(buf)-1,0);
    if(recv_size < 0)
    {
      perror("recvfrom");
      continue;
    }
    else if(recv_size == 0)
    {
      printf("peer shotdown!\n");
      close(sockfd);
      return 0;
    }

    printf("server say:%s\n",buf);
  }

  close(sockfd);
  return 0;

}
