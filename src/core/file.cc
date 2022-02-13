#include "file.hh"
#include <cstddef>
#include <vector>
//二进制方法读取文件，进行解析显示文件。
void File::Read(filesystem::path path) {
  std::vector<unsigned char> &result = m_file;
  std::ifstream nesFile{path, std::ios_base::binary};

  nesFile.seekg(0,std::ios::end);


  nesFile.seekg(std::ios::beg);
  if (!nesFile.is_open()) {
    log.error("无法打开文件");
    return;
  }

  //读取文件二进制内容
  result.insert(result.begin(), std::istreambuf_iterator<char>(nesFile),
                std::istreambuf_iterator<char>());

  size_t fileIndex = 0;
  //插入文件头
  m_header.insert(m_header.cbegin(), result.cbegin(), result.cbegin() + 16);
  fileIndex+=16;

  std::string nesFormatStart = {0x4e, 0x45, 0x53, 0x1A};

  std::string currFileStart(m_header.cbegin(), m_header.cbegin() + 4);

  if (nesFormatStart != currFileStart) {
    log.error("文件格式错误");
    return;
  }


  auto flag6 = std::bitset<8>{m_header.at(6)};
  auto hasTrainer = flag6.test(2);

  if (hasTrainer) {
    //设置trainer
    m_trainer.insert(m_trainer.cbegin(), result.cbegin() + fileIndex,
                     result.cbegin() + fileIndex + 512);
    fileIndex += 512;
  }

  //获取RPG rom大小
  auto RPGRomSize = static_cast<int>(m_header.at(4)) * 16 * 1024;
  //设置RPG rom
  m_RPGRom.insert(m_RPGRom.cbegin(), result.cbegin() + fileIndex,
                     result.cbegin() + fileIndex + RPGRomSize);
  fileIndex +=RPGRomSize;

  auto CHRRomSize = static_cast<int>(m_header.at(5)) * 8 * 1024;
  //设置CHR rom
  m_CHRRom.insert(m_CHRRom.cbegin(), result.cbegin() + fileIndex,
                     result.cbegin() + fileIndex + CHRRomSize);
  fileIndex +=CHRRomSize;

}
