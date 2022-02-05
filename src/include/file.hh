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
  std::vector<unsigned char> Read(filesystem::path path);
private:
  filesystem::path m_defaultDirectoryPath;
  const Log& log=Log::GetInstance();
};
