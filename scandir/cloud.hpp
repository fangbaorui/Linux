#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#pragma once
#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<vector>
#include<utility>
#include<unordered_map>
//#ifdef _WIN32
//#include<filesystem>
#include"httplib.h"
#include<windows.h>
//#else
#include<experimental/filesystem>
//#include<unistd.h>
//#endif

namespace cloud_sys{
  using namespace std;
  namespace fs=std::experimental::filesystem;
  class ScanDir{
    private:
      std::string _path;
    public:
      ScanDir(const std::string &path):_path(path){
        //目录不存在则创建
        if(!fs::exists(path))
        {
          fs::create_directories(path);
        }
        if(_path.back()!='/')
        {
          _path += '/';
        }
      }
      
      bool Scan(std::vector<std::string> *arry){
        for(auto& file :fs::directory_iterator(_path))
        {
          std::string name=file.path().filename().string();
          std::string pathname = _path + name;
          if (fs::is_directory(pathname))
              continue;
          arry->push_back(_path + name);
        }
        return true;
      }
  };
  class Util{
    public:
      static bool FileRead(const std::string &file,
          std::string *body){
        body->clear();
        std::ifstream in;
        in.open(file,std::ios::binary);
        if(in.is_open()==false){
          std::cout<<"open file failed!\n";
          return false;
        }
        uint64_t size=fs::file_size(file);
        body->resize(size);
        in.read(&(*body)[0],size);
        if(in.good()==false){
          std::cout<<"read file failed!\n";
          return false;
        }
        in.close();
        return true;
      }
      
      static bool FileWrite(const std::string &file,
          const std::string &body){
          std::ofstream out;
          out.open(file,std::ios::binary);
          if(out.is_open()==false){
            std::cout<<"open file failed!\n";
              return false;
          }
          out.write(&body[0],body.size());
          if(out.good()==false){
            std::cout<<"wrte file failed!\n";
            return false;
          }
          out.close();
          return true;
      }
      
      static int Split(const std::string &str,
          const std::string &sp,
          std::vector<std::string> * arry){
        int count =0;
        size_t pos,idx=0;
        while(1)
        {
          pos=str.find(sp,idx);
          if(pos==std::string::npos)
             break;
          arry->push_back(str.substr(idx,pos-idx));
          idx = pos+sp.size();
          count++;
        }
        if(idx<str.size())
        {
          count++;
          arry->push_back(str.substr(idx));
        }
        return count;
      }
  };
  class DataManager{
      private:
        std::string _path;
        std::unordered_map<std::string,std::string> _map;
      public:
        DataManager(const std::string& path)
            :_path(path)
        {}
        
        bool Read(){
          std::string body;
          std::vector<std::string> arry;
          if(Util::FileRead(_path,&body)==false){
             std::cout<<"read data set failed!\n";
             return false;
          }
          Util::Split(body,"\n",&arry);
          for(auto& line :arry){
            std::vector<std::string> kv;
            Util::Split(line, "=",&kv);
            _map[kv[0]]=kv[1];
          }
          return true;
        }

        bool Write(){
          std::stringstream ss;
          for(auto &it : _map)
          {
            ss<< it.first<<"="<< it.second <<"\n";
          }
          if(Util::FileWrite(_path,ss.str())==false){
            std::cout<<"write data set failed!\n";
            return false;
          }
          return true;
        }

        bool Exists(const std::string &key){
          auto it = _map.find(key);
          if(it==_map.end())
          {
            return false;
          }
          return true;
        }

        bool AddOrMod(const std::string &key,
            const std::string &val){

          _map[key]=val;
          return true;
        }
        bool Del(const std::string &key,
            const std::string &val){
          auto it = _map.find(key);
          if(it==_map.end())
          {
            std::cout<<key<<"not exists!\n";
            return false;
          }
          _map.erase(it);
          return true;
        }

        bool Get(const std::string &key,std::string *val){
           auto it = _map.find(key);
           if(it == _map.end())
           {
              std::cout<<key<<"not exists!\n";
              return false;
           }
           *val = _map[key];
           return true;
        }
  };

#define LISTEN_DIR "./scandir"
#define CONFIG_FILE "./data.conf"
  class Client 
  {
  private:
      ScanDir _scan;  
      DataManager _data;  
      httplib::Client* _client;
  public:
      Client(const std::string &host,int port)
          :_scan(LISTEN_DIR), _data(CONFIG_FILE)
      {
          _client = new httplib::Client(host, port);
      }
      std::string GetIdentifier(const std::string& path)
      {
          uint64_t mtime, fsize;
          fsize = fs::file_size(path);
          auto time_type = std::experimental::filesystem::last_write_time(path);
          mtime = decltype(time_type)::clock::to_time_t(time_type);
          
          std::stringstream ss;
          ss << fsize << mtime;
          return ss.str();
      }

      bool Scan(std::vector<std::pair<std::string,std::string>>* arry)
      {
          std::vector<std::string> files;
          _scan.Scan(&files);  
          for (auto& file : files)
          {
              std::string identfier = GetIdentifier(file);
              if (_data.Exists(file) == false)
              {
                  arry->push_back(std::make_pair(file, identfier));
                  continue;
              }
              std::string old;
              _data.Get(file, &old);
              if (old == identfier)
              {
                  continue;
              }
              arry->push_back(std::make_pair(file, identfier));
          }
          return true;
      }

      bool Upload(const std::string& path)
      {
          httplib::MultipartFormData file;
          file.name = "file";
          file.content_type = "application/octet-stream";
          fs::path p(path);
          file.filename = p.filename().string();
          Util::FileRead(path, &file.content);

          httplib::MultipartFormDataItems items;
          items.push_back(file);
          auto rsp = _client->Post("/multipart", items);
          if (rsp && rsp->status == 200)
          {
              std::cout << "uploat success\n";
              return true;
          }
          std::cout << "uploat failed\n";
          return false;
      }

      bool Start()
      {
          _data.Read();
          while (1)
          {
              std::vector<std::pair<std::string, std::string>> arry;
              Scan(&arry);
              for (auto& file : arry)
              {
                  std::cout << file.first << "need upload\n";
                  if(Upload(file.first) == false)
                      continue;
                  
                  _data.AddOrMod(file.first, file.second);
                  _data.Write();
              }
              Sleep(1000);
          }
          return true;
      }
  };
}





//class Client
//{
//private:
//    ScanDir _scan;  //给与监控的目录路径
//    DataManager _data;  //给与配置文件的路径
//    httplib::Client* _client;
//public:
//    Client(const std::string& host, int port)
//        :_scan(LISTEN_DIR, _data(CONFIG_FILE))
//    {
//        _client = new httplib::Client(host, port);
//    }
//    std::string GetIdentifier(const std::string& path)
//    {
//        uint64_t mtime, fsize;
//        fsize = fs::file_size(path);
//        auto time_type = std::experimental::filesystem::last_write_time(path);
//        mtime = decltype(time_type)::clock::to_time_t(time_type);
//
//        std::stringstream ss;
//        ss << fsize << mtime;
//        return ss.str();
//    }
//
//    //获取需要备份的文件信息
//    bool Scan(std::vector<std::pair<std::string, std::string>>* arry)
//    {
//        //获取文件时间属性，以及大小组织文件唯一标识
//        std::vector<std::string> files;
//        _scan.Scan(&files);  //获取当前目录下所有文件--当前目录深度为1
//        //与历史备份信息进行比较---1.是否存在 2.是否一致
//        for (auto& file : files)
//        {
//            std::string identfier = GetIdentifier(file);
//            if (_data.Exists(file) == false)
//            {
//                //不存在历史信息则标识新增
//                arry->push_back(std::make_pair(file, identfier));
//                continue;
//            }
//            std::string old;
//            _data.Get(file, &old);
//            if (old == identfier)
//            {
//                //与历史信息是否一致
//                continue;
//            }
//            arry->push_back(std::make_pair(file, identfier));
//        }
//        return true;
//    }
//    //上传指定的文件
//    bool Upload(const std::string& path)
//    {
//        httplib::MultipartFormData file;
//        file.name = "file";
//        file.content_type = "application/octet-stream";
//        fs::path p(path);
//        file.filename = p.filename().string();
//        Util::FileRead(path, &file.content);
//
//        httplib::MultipartFormDataItems items;
//        items.push_back(file);
//        auto rsp = _client->Post("/multipart", items);
//        if (rsp && rsp->status == 200)
//            return true;
//        return false;
//    }
//
//    bool Start()
//    {
//        //1.获取配置文件信息
//        _data.Read();
//        while (1)
//        {
//            //2.通过Scan接口获取到需要备份的文件路径名称---数组
//            std::vector<std::pair<std::string, std::string>> arry;
//            Scan(&arry);
//            //遍历需要备份的文件信息，然后逐个进行备份
//            for (auto& file : arry)
//            {
//                std::cout << file.first << "need upload\n";
//                //通过Upload接口对需要备份的文件进行上传备份
//                if (Upload(file.first) == false)
//                    continue;//上传失败则不能修改备份信息，等下次遍历这个文件就会检测不一致重新进行上传
//                // 
//                //记录备份成功的文件的备份信息
//                _data.AddOrMod(file.first, file.second);
//                //修改持久化存储信息
//                _dta.Write();
//            }
//            Sleep(1000);
//        }
//        return true;
//    }
//};


