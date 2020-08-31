#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QJsonDocument>
#include <QMainWindow>
#include <QtNetwork>

class PlayerA : public QMainWindow {
  Q_OBJECT

 public:
  PlayerA(QWidget *parent = nullptr);
  ~PlayerA();

  void firstCampaign(int n);

 private:
  // 连接
  bool Bconnected_ = false;
  bool Cconnected_ = false;
  QTcpServer *serverA_ = nullptr;
  QTcpSocket *socketAB_ = nullptr;
  QTcpSocket *socketAC_ = nullptr;
  const int portA_ = 10080;

  // 出牌顺序是...->A->B->C->A->...
  // 地主为A：0，B：1，C：2
  int landlord_ = 0;

 public slots:
  void handleConnections();
  void socketReadDataFromB();
  void socketReadDataFromC();
  void socketDisconnectedFromB();
  void socketDisconnectedFromC();

  void startCampaign();

 signals:
  // void thereAreThreePeople();
};
#endif  // MAINWINDOW_H
