#pragma once

#include <iostream>

#include "../connectinfo.hpp"

#include "UserManager.hpp"

#include "MsgPool.hpp"





#define UDPTHREADCOUNT 1


class TcpNewConnect

{

  public:

    TcpNewConnect(int newsockfd)

    {

      new_sock_ = newsockfd;


    }



    ~TcpNewConnect()

    {




    }



    int GetNewSockfd()

    {

      return new_sock_;


    }



    void SetThisPointer(void* chatsvr)

    {

      server_ = chatsvr;


    }



    void* GetThisPointer()

    {

      return server_;


    }

  private:

    int new_sock_;



    void* server_;


};





class ChatSvr

{

  public:

    ChatSvr()

    {

      tcp_sockfd_ = -1;

      tcp_port_ = TCPPORT;

      um_ = NULL;



      udp_sockfd_ = -1;

      udp_port_ = UDPPROT;



      msg_pool_ = NULL;


    }



    ~ChatSvr()

    {

      if (tcp_sockfd_ >= 0)

      {

        close(tcp_sockfd_);


      }


    }



    int InitSvr()

    {



      um_ = new UserManager();

      if (um_ == NULL)

      {

        printf("初始化用户管理失败\n");

        return -1;


      }



      msg_pool_ = new MsgPool();

      if (msg_pool_ == NULL)

      {

        printf("初始化消息池失败\n");

        return -1;


      }



      tcp_sockfd_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

      if (tcp_sockfd_ < 0)

      {

        perror("socket");

        return -1;


      }



      struct sockaddr_in addr;

      addr.sin_family = AF_INET;

      addr.sin_port = htons(tcp_port_);

      addr.sin_addr.s_addr = inet_addr("192.168.154.128");



      int ret = bind(tcp_sockfd_, (struct sockaddr*)&addr, sizeof(addr));

      if (ret < 0)

      {

        perror("bind");

        return -2;


      }



      ret = listen(tcp_sockfd_, 5);

      if (ret < 0)

      {

        perror("listen");

        return -3;


      }


      udp_sockfd_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

      if (udp_sockfd_ < 0)

      {

        perror("socket");

        return -5;


      }



      struct sockaddr_in udp_addr;

      udp_addr.sin_family = AF_INET;

      udp_addr.sin_port = htons(udp_port_);

      addr.sin_addr.s_addr = inet_addr("192.168.154.128");



      ret = bind(udp_sockfd_, (struct sockaddr*)&udp_addr, sizeof(udp_addr));

      if (ret < 0)

      {

        perror("bind");

        return -6;


      }

      return 0;


    }



    int StartThread()

    {



      for (int i = 0; i < UDPTHREADCOUNT; i++)

      {

        pthread_t tid;

        int ret = pthread_create(&tid, NULL, SendUdpMsgStart, (void*)this);

        if (ret < 0)

        {

          perror("pthread_create");

          return -1;


        }



        ret = pthread_create(&tid, NULL, RecvUdpMsgStart, (void*)this);

        if (ret < 0)

        {

          perror("pthread_create");

          return -1;


        }


      }


      while (1)

      {

        struct sockaddr_in peer_addr;

        socklen_t peer_addrlen = sizeof(peer_addr);

        int newsockfd = accept(tcp_sockfd_, (struct sockaddr*)&peer_addr, &peer_addrlen);

        if (newsockfd < 0)

        {


          perror("accept");

          printf("continue accept...\n");

          continue;


        }


        TcpNewConnect* tnc = new TcpNewConnect(newsockfd);

        if (tnc == NULL)

        {

          close(newsockfd);

          continue;


        }



        tnc->SetThisPointer((void*)this);



        pthread_t tid;

        int ret = pthread_create(&tid, NULL, RegiAndLoginStart, (void*)tnc);

        if (ret < 0)

        {

          delete tnc;

          close(newsockfd);

          continue;


        }


      }


    }



    static void* RegiAndLoginStart(void* arg)

    {

      pthread_detach(pthread_self());



      TcpNewConnect* tnc = (TcpNewConnect*)arg;

      ChatSvr* cs = (ChatSvr*)(tnc->GetThisPointer());



      int newsockfd = tnc->GetNewSockfd();



      char type = -1;

      while (1)

      {

        ssize_t recv_size = recv(newsockfd, &type, 1, 0);

        if (recv_size < 0)

        {

          continue;


        }

        else if (recv_size == 0)

        {

          close(newsockfd);

          delete tnc;

          pthread_exit(NULL);


        }

        else

        {

          break;


        }


      }



      int ret = -100;

      uint32_t user_id;

      switch (type)

      {

        case Register:

          {

            ret = cs->DealRegister(newsockfd, &user_id);

            break;


          }

        case Login:

          {

            ret = cs->DealLogin(newsockfd, &user_id);

            break;


          }

        default:

          break;



      }



      struct RespInfo rsp;

      rsp.status_ = ret;

      if (ret == RegisterFailed)

      {

        rsp.userid_ = 0;


      }

      else

      {

        rsp.userid_ = user_id;


      }



      send(newsockfd, &rsp, sizeof(rsp), 0);



      return NULL;


    }





    int DealRegister(int newsockfd, uint32_t* user_id)

    {

      struct RegInfo ri;

      while (1)

      {

        ssize_t recv_size = recv(newsockfd, &ri, sizeof(ri), 0);

        if (recv_size < 0)

        {

          continue;


        }

        else if (recv_size == 0)

        {

          close(newsockfd);

          return -2;


        }

        else

        {

          break;


        }


      }



      int ret = um_->RegisterUserInfo(ri, user_id);

      if (ret < 0)

      {

        return RegisterFailed;


      }



      return RegisterSuccess;


    }





    int DealLogin(int newsockfd, uint32_t* userid)

    {

      struct LoginInfo li;

      while (1)

      {

        ssize_t recv_size = recv(newsockfd, &li, sizeof(li), 0);

        if (recv_size < 0)

        {

          continue;


        }

        else if (recv_size == 0)

        {

          close(newsockfd);

          return -2;


        }

        else

        {

          break;


        }


      }



      *userid = li.userid_;


      int ret = um_->LoginCheck(li);

      if (ret < 0)

      {

        return LoginFailed;


      }

      return LoginSuccess;


    }





    static void* RecvUdpMsgStart(void* arg)

    {

      pthread_detach(pthread_self());

      ChatSvr* cs = (ChatSvr*)arg;


      while (1)

      {

        char buf[10240] = { 0  };

        struct sockaddr_in udp_peeraddr;

        socklen_t len = sizeof(udp_peeraddr);

        recvfrom(cs->udp_sockfd_, buf, sizeof(buf) - 1, 0, (struct sockaddr*)&udp_peeraddr, &len);



        string msg;

        msg.assign(buf, strlen(buf));



        UdpMsg um;

        um.deserialize(msg);



        cout << um.nickname_ << endl;

        cout << um.school_ << endl;

        cout << um.msg_ << endl;

        cout << um.user_id_ << endl;



        int ret = cs->um_->IsLogin(um.user_id_, &udp_peeraddr, len);

        if (ret < 0)

        {

          continue;


        }


        cs->msg_pool_->Push(msg);


      }

      return NULL;


    }



    static void* SendUdpMsgStart(void* arg)

    {

      pthread_detach(pthread_self());

      ChatSvr* cs = (ChatSvr*)arg;

      while (1)

      {

        string msg;

        int ret = cs->msg_pool_->Pop(&msg);

        if (ret < 0)

        {

          continue;


        }



        printf("msg Pop\n");

        cout << msg << endl;


        vector<UserInfo> vec;

        cs->um_->GetOnlineUser(&vec);



        printf("vec.size: %d\n", vec.size());



        for (size_t i = 0; i < vec.size(); i++)

        {

          printf("send msg....");

          struct sockaddr_in addr = vec[i].GetUdpAddr();

          socklen_t addr_len = vec[i].GetUdpAddrLen();

          cs->SendUdpMsg(msg, &addr, addr_len);


        }


      }



      return NULL;


    }



    int SendUdpMsg(string& msg, struct sockaddr_in* addr, socklen_t len)

    {

      int send_size = sendto(udp_sockfd_, msg.c_str(), msg.size(), 0, (struct sockaddr*)addr, len);

      if (send_size < 0)

      {

        perror("sendto failed");

        return -1;


      }

      return 0;


    }

  private:

    int tcp_sockfd_;
    uint16_t tcp_port_;
    int udp_sockfd_;
    uint16_t udp_port_;
    UserManager* um_;
    MsgPool* msg_pool_;

};

