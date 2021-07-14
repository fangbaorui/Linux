#pragma once

#include <unistd.h>

#include <sys/socket.h>

#include <netinet/in.h>

#include <arpa/inet.h>

#include <iostream>

#include <cstring>

#include <string>

#include <jsoncpp/json/json.h>



using namespace std;




#define NICKNAMELEN 20

#define SCHOOLLEN 20

#define PASSWDLEN 20



#define TCPPORT 19899

#define UDPPROT 29898



enum ResquestType

{

  Register = 0,

  Login


};



struct RegInfo

{

  RegInfo()

  {

    memset(nickname_, '\0', sizeof(nickname_));


  }

  char nickname_[NICKNAMELEN];

  char school_[SCHOOLLEN];

  char passwd_[PASSWDLEN];


};




struct LoginInfo

{

  LoginInfo()

  {


    userid_ = 0;

    memset(passwd_, '\0', sizeof(passwd_));


  }

  uint32_t userid_;

  char passwd_[PASSWDLEN];


};




struct RespInfo

{

  int status_;

  uint32_t userid_;


};



enum ResStatus

{

  RegisterFailed = 0,

  RegisterSuccess,

  LoginFailed,

  LoginSuccess,

  Online


};



class UdpMsg

{

  public:

    void serialize(string* msg)

    {

      Json::Value v;



      v["nickname"] = nickname_;

      v["school"] = school_;

      v["msg"] = msg_;

      v["userid"] = user_id_;



      Json::FastWriter w;

      *msg = w.write(v);


    }



    void deserialize(string& msg)

    {

      Json::Reader r;

      Json::Value v;

      r.parse(msg, v);



      nickname_ = v["nickname"].asString();

      school_ = v["school"].asString();

      msg_ = v["msg"].asString();

      user_id_ = v["userid"].asUInt();


    }

  public:

    string nickname_;

    string school_;

    string msg_;

    uint32_t user_id_;


};
