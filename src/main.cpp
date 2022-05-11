
// #define OLDEMU
// #ifdef OLDEMU
// #include <triangle.hh>

// int main(){
//   HelloTriangleApplication app;
//   app.run();
//   return 0;
// }
// #else
#include "mainwindow.hh"
#include <QApplication>
#include <QLayout>
#include <QVulkanInstance>
#include <memory>
#include <QWindow>
#include <iostream>
#include <window.hh>


int main(int argc, char *argv[]) {
  auto resultcode = 0;
  try {
    QApplication app(argc, argv);

    auto qVulkanInstance=std::make_unique<QVulkanInstance>();

    //auto vulkanWindow= std::make_unique<VulkanWindow>();
    VulkanGameWindow *vulkanGameWindow=new VulkanGameWindow(qVulkanInstance.get()
                                                            );
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

// #endif
