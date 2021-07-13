#include<stdio.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<cstring>

int main()
{
  /*
   1.create socket
   2.bind
   3.接受数据
   4.发送数据
   5.关闭套接字
   */
   int sockfd = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
   if(sockfd < 0)
   {
     //文件描述符本质上就是内核当中的一个数组的下标
     perror("socket");
     return 0;
   }

   struct sockaddr_in addr;
   addr.sin_family = AF_INET;
   //知名端口：0~1023
   //3306-》mysql   1521-》Oracle
   //htons(uint16_t hostshort)
   addr.sin_port = htons(19999);
   //inet_addr
   //1.将点分十进制的IP地址转换成无符号32位的整型
   //2.将无符号32位的整数转换成网络字节序
   addr.sin_addr.s_addr = inet_addr("127.0.0.1");

   int ret = bind(sockfd,(struct sockaddr*)&addr,sizeof(addr));
   if(ret < 0)
   {
     perror("bind");
     return 0;
   }

   while(1)
   {
     char buf[1024]={0};
     struct sockaddr_in peer_addr;
     socklen_t addr_len = sizeof(peer_addr);
     ssize_t recv_size = recvfrom(sockfd,buf,sizeof(buf)-1,0,(struct sockaddr*)&peer_addr,&addr_len);
     if(recv_size <= 0)
     {
       continue;
     }
     printf("client say:%s\n",buf);
     memset(buf,'\0',sizeof(buf));

     //发送数据
     printf("please enter msg:");
     fflush(stdout);
     scanf("%s",buf);
       
     sendto(sockfd,buf,strlen(buf),0,(struct sockaddr*)&peer_addr,sizeof(peer_addr));
   }

   close(sockfd);
   return 0;

}
