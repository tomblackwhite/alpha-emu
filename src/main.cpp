#include "file.hh"
#include "log.hh"
#include <cstddef>
#include <cstdlib>
#include <filesystem>
#include <ios>
int main() {
  const auto log = Log::GetInstance();
  log.info("hello world");
  std::string homePath = std::getenv("HOME");
  log.info(homePath);
  filesystem::path filePath = homePath + "/MARIO.NES";
  File file;
  file.Read(filePath);

  std::cout << file.m_file.size()  << ' '
            << file.m_trainer.size() + file.m_RPGRom.size() +
                   file.m_CHRRom.size();

  return 0;
}
