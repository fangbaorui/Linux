#pragma once
#include <string>
#include "../connectinfo.hpp"

struct MySelf
{
  std::string nick_name_;
  std::string school_;
  std::string passwd_;
  uint32_t user_id_;

};

using namespace std;
#define CHECK_RET(p) if(p < 0){return 0;}

class ChatCli
{
  public:
    ChatCli()
    {
      tcp_sockfd_ = -1;
      udp_sockfd_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
      if (udp_sockfd_ < 0)
      {
        printf("create udp sockfd failed\n");
        exit(1);

      }
      vec_.clear();

    }

    ~ChatCli()
    {


    }

    int CreateSock()
    {
      tcp_sockfd_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
      if (tcp_sockfd_ < 0)
      {
        perror("socket");
        return -1;

      }
      return 0;

    }

    int ConnectToSvr(const string& ip)
    {
      struct sockaddr_in dest_addr;
      dest_addr.sin_family = AF_INET;
      dest_addr.sin_port = htons(TCPPORT);
      dest_addr.sin_addr.s_addr = inet_addr(ip.c_str());
      int ret = connect(tcp_sockfd_, (struct sockaddr*)&dest_addr, sizeof(dest_addr));
      if (ret < 0)
      {
        perror("connect");
        printf("Ip : %s, port: %d\n", ip.c_str(), TCPPORT);
        return -1;

      }

      return 0;

    }

    int RegiToSvr(const string& ip)
    {
      int ret = CreateSock();
      if (ret < 0)
      {
        return -1;

      }

      ret = ConnectToSvr(ip);
      if (ret < 0)
      {
        return -1;

      }

      struct RegInfo ri;
      printf("please enter your nickname:");
      fflush(stdout);
      cin >> ri.nickname_;
      me_.nick_name_ = ri.nickname_;
      printf("please enter your school:");
      fflush(stdout);
      cin >> ri.school_;
      me_.school_ = ri.school_;

      while (1)
      {
        string one_passwd, two_passwd;
        printf("please enter your passwd:");
        fflush(stdout);
        cin >> one_passwd;

        printf("please retry enter your passwd:");
        fflush(stdout);
        cin >> two_passwd;

        if (one_passwd == two_passwd)
        {
          strcpy(ri.passwd_, one_passwd.c_str());
          me_.passwd_ = ri.passwd_;
          break;

        }

      }

      char type = Register;
      ssize_t send_size = send(tcp_sockfd_, &type, 1, 0);
      if (send_size != 1)
      {
        printf("send failed");
        return -1;

      }
      send_size = send(tcp_sockfd_, &ri, sizeof(ri), 0);
      if (send_size < 0)
      {
        printf("send failed: struct RegInfo\n");
        return -2;

      }

      struct RespInfo rsp;
      ssize_t recv_size = recv(tcp_sockfd_, &rsp, sizeof(rsp), 0);
      if (recv_size < 0)
      {
        perror("recv");
        return -3;

      }
      else if (recv_size == 0)
      {
        close(tcp_sockfd_);
        return -4;

      }

      if (rsp.status_ == RegisterFailed)
      {
        printf("register failed\n");
        return -5;

      }
      else if (rsp.status_ == RegisterSuccess)
      {
        printf("server return your userid is %d\n", rsp.userid_);
        me_.user_id_ = rsp.userid_;
        return 0;

      }

      return -100;

    }

    int GetTcpSockfd()
    {
      return tcp_sockfd_;

    }

    int LoginToSvr(string& ip)
    {
      int ret = CreateSock();
      if (ret < 0)
      {
        perror("LoginTOSvr");
        return -1;

      }
      ret = ConnectToSvr(ip);

      if (ret < 0)
      {
        perror("LoginTOSvr");
        return -1;

      }

      uint32_t userid = 0;
      string passwd;
      printf("please enter your userid:");
      fflush(stdout);
      cin >> userid;

      printf("please enter your passwd:");
      fflush(stdout);
      cin >> passwd;
      char type = Login;
      ssize_t send_size = send(tcp_sockfd_, &type, 1, 0);
      if (send_size != 1)
      {
        printf("send failed");
        return -1;

      }
      struct LoginInfo li;
      li.userid_ = userid;
      strcpy(li.passwd_, passwd.c_str());

      send_size = send(tcp_sockfd_, &li, sizeof(li), 0);
      if (send_size <= 0)
      {
        printf("send failed, struct LoginInfo\n");
        return -1;

      }
      struct RespInfo rsp;
      ssize_t recv_size = recv(tcp_sockfd_, &rsp, sizeof(rsp), 0);
      if (recv_size < 0)
      {
        perror("recv");
        return -3;

      }
      else if (recv_size == 0)
      {
        close(tcp_sockfd_);
        return -4;

      }

      if (rsp.status_ == LoginFailed)
      {
        printf("登录失败\n");
        return -5;

      }
      else if (rsp.status_ == LoginSuccess)
      {
        return 0;

      }
      return -100;

    }

    int SendUdpMsg(string& msg)
    {
      struct sockaddr_in dest_addr;
      dest_addr.sin_family = AF_INET;
      dest_addr.sin_port = htons(UDPPROT);
      dest_addr.sin_addr.s_addr = inet_addr("192.168.154.128");
      sendto(udp_sockfd_, msg.c_str(), msg.size(), 0, (struct sockaddr*)&dest_addr, sizeof(dest_addr));
      return 0;

    }

    int RecvUdpMsg(string* msg)
    {
      char buf[10240] = { 0  };
      recvfrom(udp_sockfd_, buf, sizeof(buf) - 1, 0, NULL, NULL);
      msg->assign(buf, strlen(buf));
      return 0;

    }

    std::vector<UdpMsg>& GetVec()
    {
      return vec_;

    }

    MySelf& GetMe()
    {
      return me_;

    }

  private:
    int tcp_sockfd_;
    int udp_sockfd_;
    std::vector<UdpMsg> vec_;
    MySelf me_;

};
