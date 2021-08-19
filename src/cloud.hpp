#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <experimental/filesystem>
#include <unistd.h>
#include <sys/stat.h>
#include "httplib.h"
#include "bundle.h"

namespace cloud_sys{
    namespace fs=std::experimental::filesystem;
    class ScanDir{
        private:
            std::string _path;
        public:
            ScanDir(const std::string &path):_path(path){
                //目录不存在则创建
                if (!fs::exists(path)) {
                    fs::create_directories(path);
                }
                if (_path.back() != '/') {
                    _path += '/';
                }
            }
            bool Scan(std::vector<std::string> *arry) {
                for(auto &file:fs::directory_iterator(_path)){
                    std::string name;
                    name = file.path().filename().string();
                    arry->push_back(_path + name);
                }
                return true;
            }
    };
    class Util{
        public:
        static bool RangeRead(const std::string &file, std::string *body, int *start, int *end){
            body->clear();
            std::ifstream in;
            in.open(file, std::ios::binary);
            if (in.is_open() == false){
                std::cout << "open file failed!\n";
                return false;
            }
            uint64_t fsize = fs::file_size(file);
            uint64_t rlen; 
            if (*end == -1) {
                *end = fsize - 1;
            }
            rlen = *end - *start + 1;
            in.seekg(*start, std::ios::beg);
            body->resize(rlen);
            in.read(&(*body)[0], rlen);
            if (in.good() == false) {
                std::cout << "read file failed!\n";
                return false;
            }
            in.close();
            return true;
        }
        static bool FileRead(const std::string &file, std::string *body) {
            body->clear();
            std::ifstream in;
            in.open(file, std::ios::binary);
            if (in.is_open() == false){
                std::cout << "open file failed!\n";
                return false;
            }
            uint64_t size = fs::file_size(file);
            body->resize(size);
            in.read(&(*body)[0], size);
            if (in.good() == false) {
                std::cout << "read file failed!\n";
                return false;
            }
            in.close();
            return true;
        }
        static bool FileWrite(const std::string &file, 
                const std::string &body) {
            std::ofstream out;
            out.open(file, std::ios::binary);
            if (out.is_open() == false) {
                std::cout << "open file failed!\n";
                return false;
            }
            out.write(&body[0], body.size());
            if (out.good() == false) {
                std::cout << "write file failed!\n";
                return false;
            }
            out.close();
            return true;
        }
        static int Split(const std::string &str, const std::string &sp,
                std::vector<std::string> *arry) {
            int count = 0;
            size_t pos, idx = 0;
            while(1) {
                pos = str.find(sp, idx);
                if (pos == std::string::npos)
                    break;
                arry->push_back(str.substr(idx, pos - idx));
                idx = pos + sp.size();
                count++;
            }
            if (idx < str.size()) {
                count++;
                arry->push_back(str.substr(idx));
            }
            return count;
        }
    };
    class DataManager{
        private:
            std::string _path;
            std::unordered_map<std::string, std::string> _map;
        public:
            DataManager(const std::string &path):_path(path){}
            bool Read() {
                std::string body;
                std::vector<std::string> arry;
                if (Util::FileRead(_path, &body) == false){
                    std::cout << "read data set failed!\n";
                    return false;
                }
                Util::Split(body, "\n", &arry);
                for (auto &line : arry) {
                    std::vector<std::string> kv;
                    Util::Split(line, "=", &kv);
                    _map[kv[0]] = kv[1];
                }
                return true;
            }
            bool Write() {
                // key=val\n...
                std::stringstream ss;
                for (auto &it : _map) {
                    ss << it.first << "=" << it.second << "\n";
                }
                if (Util::FileWrite(_path, ss.str()) == false) {
                    std::cout << "write data set failed!\n";
                    return false;
                }
                return true;
            }
            bool Exists(const std::string &key) {
                auto it = _map.find(key);
                if (it == _map.end()){
                    return false;
                }
                return true;
            }
            bool AddOrMod(const std::string &key, const std::string &val) {
                _map[key] = val;
                return true;
            }
            bool Del(const std::string &key, const std::string &val) {
                auto it = _map.find(key);
                if (it == _map.end()){
                    std::cout << key << " not exists!\n";
                    return false;
                }
                _map.erase(it);
                return true;
            }
            bool Get(const std::string &key, std::string *val) {
                auto it = _map.find(key);
                if (it == _map.end()) {
                    std::cout << key << " not exists!\n";
                    return false;
                }
                *val = _map[key];
                return true;
            }
            bool GetAllName(std::vector<std::string> *arry) {
                arry->clear();
                for (auto &file : _map) {
                    arry->push_back(file.first);
                }
                return true;
            }
    };
#define BACKUP_PATH "./backup/"
#define CONFIG_PATH "./backup.conf"
    DataManager g_data(CONFIG_PATH);
    class Server{
        private:
            httplib::Server _srv;
        private:
            static void Upload(const httplib::Request &req, httplib::Response &rsp){
                std::cout << "into Upload\n";
                auto ret = req.has_file("file");//判断有没有对应name的文件上传信息
                if (ret == false) {
                    std::cout << "have no upload file\n";
                    rsp.status = 400;
                    return ;
                }
                if (fs::exists(BACKUP_PATH) == false) {//防止父目录不存在进行创建
                    fs::create_directories(BACKUP_PATH);
                }
                const auto& file = req.get_file_value("file");//获取上传的文件信息
                std::string filename = BACKUP_PATH + file.filename;//设置文件的存储路径 ./backup/test.txt
                if (Util::FileWrite(filename, file.content) == false){//向文件写入数据
                    std::cout << "write file data failed!\n";
                    rsp.status = 500;
                    return ;
                }
                g_data.AddOrMod(file.filename, file.filename);//添加备份信息
                g_data.Write();
                return;
            }
            static void List(const httplib::Request &req, httplib::Response &rsp){
                std::stringstream ss;
                ss << "<html><head><meta http-equiv='content-type' content='text/html;charset=utf-8'>";
                ss << "</head><body>";
                std::vector<std::string> arry;
                g_data.GetAllName(&arry);
                for (auto &filename : arry) {
                    //<a href='/download/main.txt'><strong>main.txt</strong></a>
                    ss << "<hr />";
                    ss << "<a href='/backup/" << filename << "'><strong>" << filename << "</strong></a>";
                }
                ss << "<hr /></body></html>";
                rsp.body = ss.str();
                rsp.set_header("Content-Type", "text/html");
                return;
            }
            static std::string GetIdentifier(const std::string &path) {
                uint64_t mtime, fsize;
                fsize = fs::file_size(path);
                auto time_type = std::experimental::filesystem::last_write_time(path);
                mtime = decltype(time_type)::clock::to_time_t(time_type);
                std::stringstream ss;
                ss << fsize << mtime;
                return ss.str();
            }
            static void Download(const httplib::Request &req, httplib::Response &rsp){
                std::string name = req.matches[1];
                std::string pathname = BACKUP_PATH + name;

                if (g_data.Exists(name)) {
                    std::string realname;
                    g_data.Get(name, &realname);
                    if (name != realname) {
                        Compress::UnPack(realname, pathname);
                        unlink(realname);
                        g_data.AddOrMod(name, name);
                    }
                }
                std::string newetag = GetIdentifier(pathname);
                uint64_t fsize = fs::file_size(pathname);
                if (req.has_header("If-Range")) {
                    std::string oldetag = req.get_header_value("If-Range");
                    if (oldetag == newetag) {
                        //断点续传--获取区间范围
                        std::cout << req.ranges[0].first << " - " << req.ranges[0].second << "\n";
                        int start = req.ranges[0].first;
                        int end = req.ranges[0].second;
                        Util::RangeRead(pathname, &rsp.body, &start, &end);
                        rsp.set_header("Content-Type", "application/octet-stream");
                        rsp.set_header("ETag", newetag);
                        std::stringstream ss;
                        ss << "bytes " << start << "-" << end << "/" << fsize;
                        rsp.set_header("Content-Range", ss.str());
                        rsp.status = 206;
                        return;
                    }
                }
                if (Util::FileRead(pathname, &rsp.body) == false) {
                    std::cout << "read file " << pathname << "failed\n";
                    rsp.status = 500;
                    return ;
                }
                rsp.set_header("Content-Type", "application/octet-stream");
                rsp.set_header("Accept-Ranges", "bytes");
                rsp.set_header("ETag", newetag);
                rsp.status = 200;
                return;
            }
        public:
            bool Start(int port = 9000) {
                g_data.Read();
                _srv.Post("/multipart", Upload);
                _srv.Get("/list", List);
                _srv.Get("/backup/(.*)", Download);
                _srv.listen("0.0.0.0", port);
                return true;
            }
    };
    class Compress{
        public:
            static bool Pack(const std::string &filename, const std::string &packname) {
                std::string body;
                Util::FileRead(filename, &body);
                Util::FileWrite(packname, bundle::pack(bundle::LZIP, body));
                return true;
            }
            static bool UnPack(const std::string &packname, const std::string &filename) {
                std::string body;
                Util::FileRead(packname, &body);
                Util::FileWrite(filename, bundle::unpack(body));
                return true;
            }
    };
#define PACK_PATH "./packdir/"
    class FileManager{
        private:
            ScanDir _scan;
            time_t _hot_time = 10;//表示多少秒没有被访问就算是非热点文件
        private:
            time_t LastAccessTime(const std::string &filename) {
                struct stat st;
                stat(filename.c_str(), &st);
                return st.st_atime;
            }
        public:
            FileManager():_scan(PACK_PATH){}
            bool Start() {
                std::vector<std::string> arry;
                _scan.Scan(&arry);
                for (auto &file : arry) {
                    time_t atime = LastAccessTime(file);
                    time_t ctime = time(NULL);
                    if ((ctime - atime) > _hot_time) {
                        fs::path fpath(file);
                        std::string pack = PACK_PATH + fpath.filename().string() + ".pack";
                        Compress::Pack(file, pack);//压缩存储文件
                        unlink(file.c_str());//删除源文件
                        g_data.AddOrMod(fpath.filename().string(), pack);//修改备份信息
                    }
                }
                return true;
            }
    };
}
