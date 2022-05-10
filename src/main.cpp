// #include "file.hh"
// #include "log.hh"
// #include "mainwindow.hh"
// #include <QApplication>
// #include "triangle.hh"
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

// #define OLDEMU
#ifdef OLDEMU
#include <triangle.hh>

int main(){
  HelloTriangleApplication app;
  app.run();
  return 0;
}
#else
#include "mainwindow.hh"
#include <QApplication>
#include <QPushButton>
#include <QVBoxLayout>
#include <QVulkanInstance>
#include <QWindow>
#include <iostream>
#include <window.hh>


int main(int argc, char *argv[]) {
  auto resultcode = 0;
  try {
    QApplication app(argc, argv);

    VulkanGameWindow *vulkanGameWindow=new VulkanGameWindow();
    MainWindow w;

    auto widget = w.centralWidget();

    auto layout = widget->layout();
    layout->addWidget(QWidget::createWindowContainer(vulkanGameWindow));
    w.show();
    resultcode = app.exec();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return resultcode;
  }

  return EXIT_SUCCESS;

}

#endif
