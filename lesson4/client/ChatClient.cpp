#include "ChatClient.hpp"
#include "ChatWindow.hpp"

using namespace std;
void Menu()
{
  printf("**********************************\n");
  printf("***1.register*******2.login*******\n");
  printf("***3.exit*********************\n");
  printf("**********************************\n");

}


int main(int argc, char* argv[])
{
  if (argc != 3)
  {
    printf("Usage [./ChatClient -ip IP地址]");
    return 0;

  }

  std::string ip;
  for (int i = 0; i < argc; i++)
  {
    if (strcmp(argv[i], "-ip") == 0 && (i + 1) < argc)
    {
      ip = argv[i + 1];

    }

  }

  if (ip == "")
  {
    printf("ip is empty!\n");
    return 0;

  }

  ChatCli* cc = new ChatCli();

  while (1)
  {
    Menu();
    printf("please enter your select:");
    fflush(stdout);

    int select = -1;
    cin >> select;
    switch (select)
    {
      case 1:
        {
          int ret = cc->RegiToSvr(ip);
          close(cc->GetTcpSockfd());
          if (ret < 0)
          {
            continue;

          }
          printf("please select login...\n");
          break;

        }
      case 2:
        {

          int ret = cc->LoginToSvr(ip);
          close(cc->GetTcpSockfd());
          if (ret < 0)
          {
            printf("login failed, please retry login\n");
            continue;
          }
          printf("login success, please chat\n");
          ChatWindow* cw = new ChatWindow();
          cw->Start(cc);
          break;

        }
      case 3:
        {
          exit(1);
          break;

        }
      default:
        {
          break;

        }

    }

  }
  return 0;

}
