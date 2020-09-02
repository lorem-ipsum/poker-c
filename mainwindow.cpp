#include "mainwindow.h"

#include <QByteArray>
#include <QLabel>
#include <QPushButton>
#include <QTime>

PlayerC::PlayerC(QWidget* parent) : QMainWindow(parent) {
  setFixedSize(1200, 800);

  // startListening button
  buttonStartRequesting = new QPushButton(this);
  buttonStartRequesting->setGeometry(376, 512, 448, 120);
  buttonStartRequesting->setFont(QFont("Helvetica", 28));
  buttonStartRequesting->setText("Start connecting...");
  connect(buttonStartRequesting, SIGNAL(clicked()), this,
          SLOT(startRequesting()));
}

PlayerC::~PlayerC() {}

void PlayerC::startRequesting() {
  socketToA_ = new QTcpSocket();
  connect(socketToA_, &QTcpSocket::readyRead, this,
          &PlayerC::socketReadDataFromA);
  connect(socketToA_, &QTcpSocket::disconnected, this,
          &PlayerC::socketDisconnectedFromA);
  socketToA_->connectToHost("localhost", portA_);
}

void PlayerC::socketReadDataFromA() {
  qDebug() << "C IS READING FROM BUFFER";
  QByteArray buffer = socketToA_->readAll();

  QString CCards = readFromBuffer(buffer, "cardsAssignedToC");
  if (!CCards.isEmpty()) {
    buttonStartRequesting->hide();
    cardsOfC_ = stringToIntArray(CCards);

    displayCards();
  }

  QString someOneHasDecidedWhetherToCampaign =
      readFromBuffer(buffer, "ifWantToCampaign");
  if (!someOneHasDecidedWhetherToCampaign.isEmpty()) {
    qDebug() << "ifWantToCampaign RECEIVED";
    int personIndex = someOneHasDecidedWhetherToCampaign.toInt() / 10;
    bool ifCampaign = someOneHasDecidedWhetherToCampaign.toInt() % 10;
    showOthersIfCampaignInfo(personIndexToPosition_[personIndex], ifCampaign);
  }

  QString doYouWantToCampaign = readFromBuffer(buffer, "doYouWantToCampaign");
  if (!doYouWantToCampaign.isEmpty()) {
    showDoYouWantToCampaignButtons();
  }

  QString commonCardsStr = readFromBuffer(buffer, "commonCards");
  if (!commonCardsStr.isEmpty()) {
    buttonStartRequesting->hide();
    for (QString str : commonCardsStr.split(".")) {
      if (!str.isEmpty()) {
        comcards_.append(str.toInt());
      }
    }
    displayCommonCards(comcards_);
  }

  QString theLandlordIs = readFromBuffer(buffer, "theLandlordIs");
  if (!theLandlordIs.isEmpty()) {
    setLandlord(theLandlordIs.toInt());
    if (landlord_ == 1) {
      cardsOfC_.append(comcards_);
      std::sort(cardsOfC_.begin(), cardsOfC_.end());
      displayCards();
    }
  }

  QString someOneHasPushedCards =
      readFromBuffer(buffer, "someOneHasPushedCards");
  if (!someOneHasPushedCards.isEmpty()) {
    lastPushCardPerson_ = 0;
    cardsOnTable_ = stringToIntArray(someOneHasPushedCards);
    showTableOnSelfScreen(cardsOnTable_);
  }

  QString chuOrBuchu = readFromBuffer(buffer, "chuOrBuchu");
  if (!chuOrBuchu.isEmpty()) {
    showChuOrBuchuBtns();
  }
}
void PlayerC::socketDisconnectedFromA() {}

void PlayerC::showOthersIfCampaignInfo(int personPosition, bool ifCampaign) {
  if (personPosition == 0) {
    QLabel* jiaoORbujiao = new QLabel(this);
    jiaoORbujiao->setText(ifCampaign ? "叫" : "不叫");
    jiaoORbujiao->setGeometry(580, 400, 40, 32);
    jiaoORbujiao->show();
  } else if (personPosition == 1) {
    QLabel* jiaoORbujiao = new QLabel(this);
    jiaoORbujiao->setText(ifCampaign ? "叫" : "不叫");
    jiaoORbujiao->setGeometry(160, 200, 40, 32);
    jiaoORbujiao->show();
  } else if (personPosition == 2) {
    QLabel* jiaoORbujiao = new QLabel(this);
    jiaoORbujiao->setText(ifCampaign ? "叫" : "不叫");
    jiaoORbujiao->setGeometry(960, 200, 40, 32);
    jiaoORbujiao->show();
  }
}

void PlayerC::showDoYouWantToCampaignButtons() {
  QPushButton* yesBtn = new QPushButton(this);
  QPushButton* noBtn = new QPushButton(this);

  yesBtn->setText("Yes");
  noBtn->setText("No");
  yesBtn->setGeometry(120, 220, 50, 50);
  noBtn->setGeometry(180, 220, 50, 50);
  yesBtn->show();
  noBtn->show();
  // doYouWantToCampaign->addButton(yesBtn);
  // doYouWantToCampaign->addButton(noBtn);

  connect(yesBtn, SIGNAL(clicked()), yesBtn, SLOT(hide()));
  connect(yesBtn, SIGNAL(clicked()), noBtn, SLOT(hide()));
  connect(noBtn, SIGNAL(clicked()), yesBtn, SLOT(hide()));
  connect(noBtn, SIGNAL(clicked()), noBtn, SLOT(hide()));

  connect(yesBtn, &QPushButton::clicked,
          [=]() { castToA("doYouWantToCampaign", "true"); });
  connect(noBtn, &QPushButton::clicked,
          [=]() { castToA("doYouWantToCampaign", "false"); });
}

void PlayerC::displayCards() {
  // 先删除之前的卡
  for (QLabel* item : cardLabels_) {
    item->hide();
  }
  cardLabels_.clear();

  // SHOW
  for (int i = 0; i < cardsOfC_.size(); ++i) {
    CardLabel* cardLabel = new CardLabel(this);
    cardLabels_.append(cardLabel);
    QPixmap cardImage =
        QPixmap("./assets/" + QString::number(cardsOfC_[i]) + ".png");
    cardImage = cardImage.scaled(QSize(330, 160), Qt::IgnoreAspectRatio,
                                 Qt::SmoothTransformation);
    cardLabel->setPixmap(cardImage);
    cardLabel->setGeometry(80 + 50 * i, 600, 360, 200);
    cardLabel->show();
  }
}

void PlayerC::displayCommonCards(QList<int> commonCards) {
  for (int i = 0; i < commonCards.size(); ++i) {
    QLabel* cardLabel = new QLabel(this);
    QPixmap cardImage =
        QPixmap("./assets/" + QString::number(commonCards[i]) + ".png");
    cardImage = cardImage.scaled(QSize(330, 160), Qt::IgnoreAspectRatio,
                                 Qt::SmoothTransformation);
    cardLabel->setPixmap(cardImage);
    cardLabel->setGeometry(500 + 50 * i, 50, 360, 200);
    cardLabel->show();
  }
}

void PlayerC::showTableOnSelfScreen(QList<int>) {
  // 先删除之前的卡
  for (QLabel* item : tableCardLabels_) {
    item->hide();
  }
  tableCardLabels_.clear();

  // SHOW
  for (int i = 0; i < cardsOnTable_.size(); ++i) {
    CardLabel* cardLabel = new CardLabel(this);
    tableCardLabels_.append(cardLabel);
    QPixmap cardImage =
        QPixmap("./assets/" + QString::number(cardsOnTable_[i]) + ".png");
    cardImage = cardImage.scaled(QSize(330, 160), Qt::IgnoreAspectRatio,
                                 Qt::SmoothTransformation);
    cardLabel->setPixmap(cardImage);
    cardLabel->setGeometry(500 + 50 * i, 250, 360, 200);
    cardLabel->show();
  }
}

void PlayerC::showChuOrBuchuBtns() {
  qDebug() << "C chu or buchu";
  QPushButton* chuBtn = new QPushButton(this);
  QPushButton* buchuBtn = new QPushButton(this);

  chuBtn->setText("出牌");
  buchuBtn->setText("不出");
  chuBtn->setGeometry(120, 220, 50, 50);
  buchuBtn->setGeometry(180, 220, 50, 50);
  chuBtn->show();
  buchuBtn->show();

  connect(buchuBtn, SIGNAL(clicked()), chuBtn, SLOT(hide()));
  connect(buchuBtn, SIGNAL(clicked()), buchuBtn, SLOT(hide()));

  buchuBtn->setDisabled(lastPushCardPerson_ == 2);
  if (lastPushCardPerson_ == 2) {
    cardsOnTable_ = {};
  }

  // btn connect
  connect(chuBtn, &QPushButton::clicked, [=]() {
    // lastPushCardPerson_ = 1;
    // cardsOnTable_ = {1};  //
    // showTableOnSelfScreen(cardsOnTable_);

    QList<int> cardsToPush;
    for (int i = 0; i < cardsOfC_.size(); ++i) {
      if (cardLabels_[i]->isMoved()) {
        cardsToPush.append(cardsOfC_[i]);
      }
    }

    // CHECK TODO

    qDebug() << "CHECKING...1";
    if (CARDS_CMP(cardsToPush, cardsOnTable_)) {
      qDebug() << "CHECKING...2";

      // C不再展示出掉的牌
      for (int i = 0; i < cardsToPush.size(); ++i) {
        cardsOfC_.removeOne(cardsToPush[i]);
      }
      displayCards();

      notifyAThatCHasJustPushedCards(cardsToPush);
      lastPushCardPerson_ = 2;
      cardsOnTable_ = cardsToPush;
      showTableOnSelfScreen(cardsOnTable_);

      chuBtn->hide();
      buchuBtn->hide();
    }
  });
  connect(buchuBtn, &QPushButton::clicked, [=]() { tellAGiveUp(); });
}

void PlayerC::notifyAThatCHasJustPushedCards(QList<int> CPushedCards) {
  castToA("CHasPushedCards", intArrayToString(CPushedCards));
}

void PlayerC::tellAGiveUp() { castToA("CHasGivenUp", "info"); }