#include "file.hh"
#include "log.hh"
#include "mainwindow.hh"
#include <QApplication>
#include "triangle.hh"
// int main(int argc , char *argv[]) {
//   const auto log = Log::GetInstance();
//   log.info("hello world");
//   std::string homePath = std::getenv("HOME");
//   log.info(homePath);
//   filesystem::path filePath = homePath + "/MARIO.NES";
//   File file;
//   file.Read(filePath);
//   std::cout << file.m_header.size() << ' ' << file.m_file.size()  << ' '
//             << file.m_trainer.size() + file.m_RPGRom.size() +
//                    file.m_CHRRom.size();

//   QApplication app(argc,argv);

//   MainWindow w;
//   w.show();
//   return app.exec();
// }



int main() {

  HelloTriangleApplication app;
  try {
    std::cout << "success run\n";
    app.run();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
