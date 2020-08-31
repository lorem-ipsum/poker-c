#include <QApplication>

#include "mainwindow.h"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  PlayerA w;
  w.show();
  return a.exec();
}
