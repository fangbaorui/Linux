#include<iostream>
#include<string>
#include<fstream>
#include"httplib.h"
using namespace httplib;
void WriteFile(const std::string &path,const std::string &body)
{
  std::ofstream outfile;
  outfile.open(path,std::ios::binary);
  outfile.write(&body[0],body.size());
  outfile.close();
}
void Upload(const Request &req,const Response &rsp)
{
  auto ret = req.has_file("file1");
  if(ret == false){
     std::cout<<"have no file\n";
     return;
  }
  const auto& file = req.get_file_value("file1");
  WriteFile(file.filename, file.content);
  //file.filename;
  //file.content_type;
  //file.content;
}
int main()
{
  Server srv;
  srv.Post("multipart",Upload);
  srv.listen("127.0.0.1",9000);
  return 0;
}
