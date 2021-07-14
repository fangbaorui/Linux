#include "ChatServer.hpp"



int main()

{

  ChatSvr cs;

  int ret = cs.InitSvr();

  if(ret < 0)

  {

    return 0;


  }

  cs.StartThread();

  return 0;


}


