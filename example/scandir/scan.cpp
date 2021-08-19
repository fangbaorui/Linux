#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<string>
#include<cstdlib> 
#include<experimental/filesystem>
#ifdef _WIN32
#include<windows.h>
#else
#include<unistd.h>
#endif
using namespace std;
namespace fs = std::experimental::filesystem;

void ScanDir(const std::string& path)
{
  if (!std::experimental::filesystem::exists(path))
  {
    std::experimental::filesystem::create_directories(path);

  }
  for (auto &file : std::experimental::filesystem::directory_iterator(path))
  {
    auto &f = file.path();
    if (std::experimental::filesystem::is_directory(f))
    {
      std::cout << f.filename().string() << "/\n";
      continue;

    }
    else
      std::cout << f.filename().string();
    uint64_t size = std::experimental::filesystem::file_size(f);
    std::cout << "\t" << size;
    auto time_type = std::experimental::filesystem::last_write_time(f);
    std::time_t t = decltype(time_type)::clock::to_time_t(time_type);
    std::cout << "\t" << std::asctime(std::localtime(&t)) << std::endl;

  }

}
int main()
{
  const char* path = "./testdir";
  while (1)
  {
    ScanDir(path);
#ifdef _WIN32
    Sleep(1000);
#else    
    sleep(1);
#endif
  }
  return 0;

}
