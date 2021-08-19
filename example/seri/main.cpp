#include<iostream>
#include<string>
#include<fstream>
#include<vector>
#include<unordered_map>
#include<sstream>
#include<experimental/filesystem>
using namespace std;
int Split(const string& str, const string& sp,vector<string> *arry)
{
  int count = 0;
  size_t pos,idx=0;
  while(1)
      {
         pos = str.find(sp, idx);
         if(pos==string::npos)
         {
            break;
         }
        string tmp = str.substr(idx,pos - idx);
        arry->push_back(tmp);
        idx = pos + sp.size();
        count++; 
      }
  if(idx!=str.size())
  {
   arry->push_back(str.substr(idx));  
  }
  return count;
}

void unseri()
{
  const char* name = "test.dat";
  std::string body;
  ifstream infile;
  infile.open(name,std::ios::binary);
  if(infile.is_open()==false)
  {
    std::cout<<"open file failed!\n";
    return;  
  }
  uint64_t size = std::experimental::filesystem::file_size(name);
  body.resize(size);
  infile.read(&body[0],size);
  infile.close();


  vector<string> arry;
  Split(body,"\n",&arry);
  for(auto& s :arry)
  {
    vector<string> kv;
    Split(s,"=",&kv);

    cout<<"["<<kv[0]<<"]=["<<kv[1]<<"]\n";;
  }
  //std::cout<<body<<std::endl;
  return;
}
void seri()
{
  unordered_map<string,string> _map={
    {"main.cpp","9913687567"},
    {"child.cpp","9913676927"},
    {"client.cpp","9913675789"},
    {"server.cpp","9913670987"}
  };
  stringstream ss;
  ss.clear();
  for(auto& info :_map)
  {
    ss<<info.first<<"="<<info.second<<"\n";
  }
  ofstream outfile;
  outfile.open("test.dat",std::ios::binary);
  outfile<<ss.str();
  outfile.close();
}

int main()
{
  //seri();
  unseri();
  return 0;
}
