#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <cstring>

#include "../connectinfo.hpp"

using namespace std;

class UserInfo
{
  public:
    UserInfo(const string& nickname, string& school, string& passwd, uint32_t userid)
    {
      nickname_ = nickname;
      school_ = school;
      passwd_ = passwd;
      userid_ = userid;

      status_ = RegisterFailed;

      //memset((char*)udp_addr_, '\0', sizeof(udp_addr_));
      //            addr_len_ = -1;
      //                    
    }

    string& GetPasswd()
    {
      return passwd_;

    }

    void SetUserStatus(int sta)
    {
      status_ = sta;

    }

    int GetUserStatus()
    {
      return status_;

    }

    void SetUdpAddr(struct sockaddr_in* udp_addr)
    {
      memcpy(&udp_addr_, udp_addr, sizeof(struct sockaddr_in));

    }

    void SetUdpAddrLen(socklen_t len)
    {
      addr_len_ = len;

    }

    struct sockaddr_in& GetUdpAddr()
    {
      return udp_addr_;

    }

    socklen_t GetUdpAddrLen()
    {
      return addr_len_;

    }

  private:
    string nickname_;
    string passwd_;
    string school_;
    uint32_t userid_;

    int status_;

    struct sockaddr_in udp_addr_;
    socklen_t addr_len_;

};

class UserManager
{
  public:
    UserManager()
    {
      map_.clear();
      prepare_id_ = 1;
      pthread_mutex_init(&map_lock_, NULL);

      online_vec_.clear();

    }
    ~UserManager()
    {
      pthread_mutex_destroy(&map_lock_);

    }

    int RegisterUserInfo(struct RegInfo& ri, uint32_t* user_id)
    {

      if(strlen(ri.passwd_) == 0)
      {
        printf("密码长度不对， 注册失败");
        return -1;

      }

      string nickname, school, passwd;
      nickname = ri.nickname_;
      school = ri.school_;
      passwd = ri.passwd_;

      pthread_mutex_lock(&map_lock_);
      UserInfo ui(nickname, school, passwd, prepare_id_);
      *user_id = prepare_id_;
      ui.SetUserStatus(RegisterSuccess);
      map_.insert(std::make_pair(prepare_id_, ui));
      prepare_id_++;
      pthread_mutex_unlock(&map_lock_);

      return 0;

    }

    int LoginCheck(struct LoginInfo& li)
    {

      if(strlen(li.passwd_) == 0)
      {
        return -1;

      }

      unordered_map<uint32_t, UserInfo>::iterator iter;

      pthread_mutex_lock(&map_lock_);
      iter = map_.find(li.userid_);
      if(iter == map_.end())
      {
        pthread_mutex_unlock(&map_lock_);
        return -2;

      }

      string send_passwd = li.passwd_;

      if(send_passwd != iter->second.GetPasswd())
      {
        return -3;

      }

      iter->second.SetUserStatus(LoginSuccess);
      pthread_mutex_unlock(&map_lock_);
      return 0;

    }

    int IsLogin(uint32_t userid, struct sockaddr_in* udp_addr, socklen_t addr_len)
    {
      pthread_mutex_lock(&map_lock_);
      unordered_map<uint32_t, UserInfo>::iterator iter = map_.find(userid);
      if(iter == map_.end())
      {
        pthread_mutex_unlock(&map_lock_);
        return -1;

      }

      int flag = -1;
      if(iter->second.GetUserStatus() <= LoginFailed)
      {
        flag = -2;

      }
      else if(iter->second.GetUserStatus() == LoginSuccess)
      {
        flag = 0;

        iter->second.SetUdpAddr(udp_addr);
        iter->second.SetUdpAddrLen(addr_len);

        iter->second.SetUserStatus(Online);

        online_vec_.push_back(iter->second);

      }

      if(iter->second.GetUserStatus() == Online)
      {
        pthread_mutex_unlock(&map_lock_);
        return 0;

      }
      pthread_mutex_unlock(&map_lock_);
      return flag;

    }

    void GetOnlineUser(vector<UserInfo>* online_user)
    {
      *online_user = online_vec_; 

    }
  private:

    unordered_map<uint32_t, UserInfo> map_;

    uint32_t prepare_id_;
    pthread_mutex_t map_lock_;

    vector<UserInfo> online_vec_;

};
