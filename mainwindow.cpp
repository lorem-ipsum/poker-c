#include "mainwindow.h"

#include <QTime>

PlayerA::PlayerA(QWidget *parent) : QMainWindow(parent) {
  setFixedSize(600, 400);

  // Wait for connections
  serverA_ = new QTcpServer();

  // Connection from B
  serverA_->listen(QHostAddress::Any, portA_);
  connect(serverA_, &QTcpServer::newConnection, this,
          &PlayerA::handleConnections);
  // serverA_->close();

  // Choose one to call landlord
  //

  // Game started
}

PlayerA::~PlayerA() {}

void PlayerA::handleConnections() {
  if (!socketAB_) {
    qDebug() << "Server: new connection from B";
    socketAB_ = serverA_->nextPendingConnection();
  } else {
    qDebug() << "Server: new connection from C";
    socketAC_ = serverA_->nextPendingConnection();
  }

  if (socketAB_ && socketAC_) {
    connect(socketAB_, &QTcpSocket::readyRead, this,
            &PlayerA::socketReadDataFromB);
    connect(socketAB_, &QTcpSocket::disconnected, this,
            &PlayerA::socketDisconnectedFromB);

    connect(socketAC_, &QTcpSocket::readyRead, this,
            &PlayerA::socketReadDataFromC);
    connect(socketAC_, &QTcpSocket::disconnected, this,
            &PlayerA::socketDisconnectedFromC);

    startCampaign();
  }
}

// 大致上是对方write时调用
void PlayerA::socketReadDataFromB() {
  // Check who is connecting
  Bconnected_ = true;
}
void PlayerA::socketReadDataFromC() {
  // Check who is connecting
  Cconnected_ = true;
}

void PlayerA::socketDisconnectedFromB() { qDebug() << "A is reading from B"; }
void PlayerA::socketDisconnectedFromC() { qDebug() << "A is reading from C"; }

void PlayerA::startCampaign() {
  QTime time = QTime::currentTime();
  qsrand(time.msec() + time.second() * 1000);
  int n = qrand() % 3;

  firstCampaign(n);
}

void PlayerA::firstCampaign(int n) {
  QJsonObject jsonObject;
  jsonObject.insert("firstCall", n);

  QJsonDocument jsonDocument;
  jsonDocument.setObject(jsonObject);
  QByteArray dataArray = jsonDocument.toJson();

  qDebug() << "Writing to B & C:" << dataArray;
  socketAB_->write(dataArray);
  socketAC_->write(dataArray);

  landlord_ = 0;
}