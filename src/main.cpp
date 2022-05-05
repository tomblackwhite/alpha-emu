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

#include "mainwindow.hh"
#include "triangle.hh"
#include <QApplication>
#include <QVulkanInstance>
#include <QWindow>
#include <QVBoxLayout>
#include <QPushButton>
#include <iostream>

int main(int argc, char *argv[]) {

  QApplication app(argc, argv);

  QVulkanInstance instance;
  if(!instance.create()){
    return 1;
  }




  QWindow *vulkanWindow = new QWindow();

  vulkanWindow->setVulkanInstance(&instance);


  MainWindow w;

  auto widget= w.centralWidget();

  auto layout = widget->layout();
   layout->addWidget(QWidget::createWindowContainer(vulkanWindow));
  w.show();
  auto resultcode = app.exec();


  return resultcode;
}
