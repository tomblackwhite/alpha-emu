#pragma once
#include <filesystem>
#include <spdlog/spdlog.h>
namespace filesystem = std::filesystem;
class Log {
public:
  // using TextSink = sinks::synchronous_sink<sinks::text_ostream_backend>;
  //初始化log
  Log(filesystem::path filePath = "") : m_filePath(filePath) {}
  void info(std::string_view str) const { spdlog::info("{}", str); }
  void warn(std::string_view str) const { spdlog::warn("{}", str); }
  void error(std::string_view str) const { spdlog::error("{}", str); }
  void debug(std::string_view str) const { spdlog::debug("{}", str); }

  static Log& GetInstance(){
    static Log log;
    return log;
  }
private:
  filesystem::path m_filePath;
};
