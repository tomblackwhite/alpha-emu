#include "file.hh"
//二进制方法读取文件，进行解析显示文件。
std::vector<unsigned char> File::Read(filesystem::path path) {
  std::vector<unsigned char> result;
  std::ifstream nesFile{path, std::ios_base::binary};
  if (!nesFile.is_open()) {
    log.error("无法打开文件");
    return result;
  }

  //读取文件二进制内容
  result.insert(result.begin(), std::istream_iterator<unsigned char>(nesFile),
             std::istream_iterator<unsigned char>());
  return result;
}
