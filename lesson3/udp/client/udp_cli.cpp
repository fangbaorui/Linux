#include<stdio.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<cstring>


int main()
{
  /*
   * 1.create socket
   * 2.bind
   * 3.发送数据
   * 4.接受数据
   * 5.关闭套接字
   */

   int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
   if(sockfd < 0)
   {
     perror("socket");
     return 0;
   }

   while(1)
   {
     char buf[1024] = {0};
     //发送数据
     printf("please send input sendto svr msg:");
     fflush(stdout);



     //memset(buf,'\0',sizeof(buf));
     scanf("%s",buf);

     //组织的服务器的地址信息结构
     struct sockaddr_in dest_addr;
     dest_addr.sin_family = AF_INET;
     dest_addr.sin_port = htons(19999);
     dest_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

     sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr*)&dest_addr, sizeof(dest_addr));
     
     memset(buf,'\0',sizeof(buf));


     ssize_t recv_size = recvfrom(sockfd, buf, sizeof(buf)-1, 0, NULL, NULL);
     if(recv_size <= 0)
     {
        continue;
     }
     printf("server say:%s\n",buf);
   }

   close(sockfd);
   return 0;
}
