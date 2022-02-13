#pragma once
#include <filesystem>
#include <cstddef>
#include <fmt/format.h>
#include <fstream>
#include <ios>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>
#include "log.hh"
#include <cstddef>
#include <algorithm>
namespace filesystem=std::filesystem;

class File{
public:
  void Read(filesystem::path path);
private:
  filesystem::path m_defaultDirectoryPath;

  public:

  std::vector<unsigned char> m_file;

  //nes文件header
  std::vector<unsigned char> m_header;

  //trainer
  std::vector<unsigned char> m_trainer;

  //rpg rom data
  std::vector<unsigned char> m_RPGRom;

  //chr rom data
  std::vector<unsigned char> m_CHRRom;

  //playChoice inst-rom
  std::vector<unsigned char> m_InstRom;

  //playChoice PROM
  std::vector<unsigned char> m_PRom;

  //单例获取log
  const Log& log=Log::GetInstance();
};
