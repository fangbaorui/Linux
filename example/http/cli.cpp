#include<iostream>
#include<fstream>
#include"httplib.h"
using namespace httplib;

void ReadFile(const std::string &path,std::string *body)
{
  std::ifstream infile;
  infile.open(path);
  infile.seekg(0,std::ios::end);//跳转到文件末尾
  long len = infile.tellg();//获取偏移量-文件长度
  infile.seekg(0,std::ios::beg);//跳转到文件起始位置
  body->resize(len);//申请空间
  infile.read(&(*body)[0],len);//读取文件数据
  infile.close();//关闭文件
}
int main()
{
  const char *name = "./out.mp3";
  Client cli("127.0.0.1",9000);
  MultipartFormDataItems items;
  MultipartFormData file1;
  file1.name = "file1";
  file1.filename = "out_test.mp3";
  file1.content_type = "application/octet-stream";
  ReadFile(name,&file1.content);

  items.push_back(file1);
  cli.Post("/multipart",items);
  return 0;
}
