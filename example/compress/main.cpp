#include<iostream>
#include<fstream>
#include<experimental/filesystem>
#include "bundle.h"
void uncompress()
{
  const char *name="./bundle.cpp.lz";
  const char* name1 = "./bundle.cc";
  uint64_t size = std::experimental::filesystem::file_size(name);
  std::string body;
  body.resize(size);
  std::ifstream infile;
  infile.open(name,std::ios::binary);
  infile.read(&body[0],size); 
  infile.close();  

  std::string str = bundle::unpack(body);
  std::ofstream outfile;
  outfile.open(name1,std::ios::binary);
  outfile.write(&str[0],str.size());
  outfile.close();
}
void compress()
{
  const char* name = "./bundle.cpp";
  uint64_t size = std::experimental::filesystem::file_size(name);
  std::string body;
  body.resize(size);
  std::ifstream infile;
  infile.open(name,std::ios::binary);
  infile.read(&body[0],size);
  infile.close();

  std::string str =bundle::pack(bundle::LZIP, body);
  std::ofstream outfile;
  outfile.open("./bundle.cpp.lz",std::ios::binary);
  outfile.write(&str[0],str.size());
  outfile.close();
}
int main()
{

  uncompress();
  return 0;
}
