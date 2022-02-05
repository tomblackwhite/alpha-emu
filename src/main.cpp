#include "log.hh"
#include "file.hh"
#include <cstddef>
#include <cstdlib>
#include <filesystem>
#include <ios>
int main() {
  const auto log=Log::GetInstance();
  log.info("hello world");
  std::string homePath=std::getenv("HOME");
  log.info(homePath);
  filesystem::path filePath=homePath+"/MARIO.NES";
  File file;
  auto re=file.Read(filePath);
  for(auto elem:re){
    //std::cout << std::hex <<(int) elem;
  }
  return 0;
}
