
#include<ncurses.h>

#include "ChatClient.hpp"



class ChatWindow;



class Pram

{

  public:

    Pram(int thread_num, ChatCli* cc, ChatWindow* cw)

    {

      thread_num_ = thread_num;

      cc_ = cc;

      cw_ = cw;


    }

  public:

    int thread_num_;

    ChatCli* cc_;

    ChatWindow* cw_;


};



class ChatWindow

{

  public:

    ChatWindow()

    {

      header_ = NULL;

      output_ = NULL;

      user_list_ = NULL;

      input_ = NULL;

      vec_.clear();



      pthread_mutex_init(&lock_win_, NULL);


    }



    ~ChatWindow()

    {

      if(header_)

      {

        delwin(header_);


      }

      if(output_)

      {

        delwin(output_);


      }

      if(user_list_)

      {

        delwin(user_list_);


      }

      if(input_)

      {

        delwin(input_);


      }



      pthread_mutex_destroy(&lock_win_);

      endwin();


    }



    int Start(ChatCli* cc)

    {

      initscr();

      for(int i = 0; i < 4; i++)

      {

        pthread_t tid;



        Pram* p = new Pram(i, cc, this);

        if(p == NULL)

        {

          printf("内存不够...\n");

          exit(1);


        }



        int ret = pthread_create(&tid, NULL, WindowStart, (void*)p);

        if(ret < 0)

        {

          perror("pthrad_create");

          exit(1);


        }

        vec_.push_back(tid);


      }



      for(int i = 0; i < 4; i++)

      {

        pthread_join(vec_[i], NULL);


      }

      return 0;


    }



    static void* WindowStart(void* arg)

    {

      Pram* p = (Pram*)arg;

      ChatCli* cc = p->cc_;

      ChatWindow* cw = p->cw_;

      int thread_num = p->thread_num_;



      switch(thread_num)

      {

        case 0:

          {

            //header
            //
            //                        cw->RunHeader();
            //
            //                                                break;
            //
            //                                                                    
          }

        case 1:

          {

            //output
            //
            //                        cw->RunOutput(cc);
            //
            //                                                break;
            //
            //                                                                    
          }

        case 2:

          {

            //user_list
            //
            //                        cw->RunUserList(cc);
            //
            //                                                break;
            //
            //                                                                    
          }

        case 3:

          {

            //input
            //
            //                        cw->RunInput(cc);
            //
            //                                                break;
            //
            //                                                                    
          }


      }



      while(1)

      {

        sleep(1);


      }


    }



    void RefreshWin(WINDOW* win)

    {

      pthread_mutex_lock(&lock_win_);

      wrefresh(win);

      pthread_mutex_unlock(&lock_win_);


    }



    void RunHeader()

    {

      WINDOW* old_header = NULL;

      while(1)

      {

        header_ = newwin(LINES/5, COLS, 0, 0);

        if(old_header)

        {

          delwin(old_header);


        }

        box(header_, 0, 0);

        RefreshWin(header_);



        string msg = "welcome to our system";

        int y, x;

        getmaxyx(header_, y, x);

        mvwaddstr(header_, y/2, (x - msg.size())/2, msg.c_str());

        RefreshWin(header_);



        old_header = header_;

        sleep(1);


      }


    }



    void RunUserList(ChatCli* uc)

    {

      WINDOW* old_user_list = NULL;

      int line = 1;

      int cols = 1;

      while(1)

      {

        user_list_ = newwin((3*LINES)/5, COLS/4, LINES/5, (3*COLS)/4);

        box(user_list_, 0, 0);

        RefreshWin(user_list_);



        if(old_user_list)

        {

          delwin(old_user_list);


        }



        std::vector<UdpMsg> vec = uc->GetVec();



        for(size_t i = 0; i < vec.size(); i++)

        {

          std::string msg;

          msg += vec[i].nickname_;

          msg += ":";

          msg += vec[i].school_;

          mvwaddstr(user_list_, line + i, cols, msg.c_str());

          RefreshWin(user_list_);


        }

        old_user_list = user_list_;

        sleep(1);


      }


    }



    void RunInput(ChatCli* uc)

    {



      WINDOW* old_input = NULL;

      while(1)

      {

        input_ = newwin(LINES/5, COLS, (LINES*4)/5, 0);

        box(input_, 0, 0);

        RefreshWin(input_);



        if(old_input)

        {

          delwin(old_input);


        }



        std::string tips = "please enter msg# ";

        mvwaddstr(input_, 1, 1, tips.c_str());

        RefreshWin(input_);



        char buf[10240] = {0};

        wgetnstr(input_, buf, sizeof(buf) - 1);



        UdpMsg um;

        um.nickname_ = uc->GetMe().nick_name_;

        um.school_ = uc->GetMe().school_;

        um.user_id_ = uc->GetMe().user_id_;

        um.msg_.assign(buf, strlen(buf));



        std::string send_msg;

        um.serialize(&send_msg);



        uc->SendUdpMsg(send_msg);

        old_input = input_;



        sleep(1);


      }


    }



    void DrowOutput()

    {

      output_ = newwin((LINES*3)/5, (3*COLS)/4, LINES/5, 0);

      box(output_, 0, 0);

      RefreshWin(output_);




    }



    void RunOutput(ChatCli* uc)

    {

      int line = 1;

      int cols = 1;



      DrowOutput();

      int y, x;

      getmaxyx(output_, y, x);



      std::string msg;

      while(1)

      {



        msg.clear();

        uc->RecvUdpMsg(&msg);



        UdpMsg um;

        um.deserialize(msg);



        std::string show_msg;

        show_msg += um.nickname_;

        show_msg += "-";

        show_msg += um.school_;

        show_msg += ": ";

        show_msg += um.msg_;



        if(line >= y - 2)

        {

          DrowOutput();

          line = 1;




        }

        mvwaddstr(output_, line, cols, show_msg.c_str());

        RefreshWin(output_);



        line++;



        std::vector<UdpMsg> vec = uc->GetVec();

        int flag = 1;



        for(size_t i = 0; i < vec.size(); i++)

        {

          if(um.user_id_ == vec[i].user_id_)

          {

            flag = 0;

            break;


          }

          flag = 1;


        }



        if(flag == 1)

        {

          uc->GetVec().push_back(um); 


        }


      }


    }

  private:

    WINDOW* header_;

    WINDOW* output_;

    WINDOW* user_list_;

    WINDOW* input_;



    pthread_mutex_t lock_win_;



    vector<pthread_t> vec_;


};

