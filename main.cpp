#include "mainwindow.h"

#include <QApplication>
int
main(int argc, char* argv[])
{
   qInstallMessageHandler(outputMessage);
   QApplication a(argc, argv);
   MainWindow w;
   w.show();
   return a.exec();
}
