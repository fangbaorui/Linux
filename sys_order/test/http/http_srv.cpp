#include"httplib.h"
#include<iostream>
void hello(const httplib::Request &req,httplib::Response &rsp){
  std::cout << req.method << std::endl;
  std::cout << req.path << std::endl;
  std::cout << req.body << std::endl;
  auto it = req.headers.begin();
  for(; it != req.headers.end(); it++)
  {
    std::cout << it->first << " = " << it->second << std::endl;
  }
  
  it = req.params.begin();
  for(; it != req.params.end(); it++)
  {
    std::cout << it->first << " = " << it->second << std::endl;
  }

  rsp.status = 200;
  rsp.body = "<html><body><h1>Hello Bit</h1></body></html>";
  rsp.set_header("Content - Type", "text/html");
}
int main()
{
   httplib::Server srv;
   
   srv.Get("/hello",hello);
   srv.set_base_dir("./wwwroot");
   srv.listen("0.0.0.0",9000);
   return 0;
}




