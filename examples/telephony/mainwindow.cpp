#include "mainwindow.h"
#include <qmobilityglobal.h>
#include "ui_mainwindow.h"
#include <QtCore/QDebug>
#include <qtelephony.h>
#include <qtelephonycalllist.h>

using namespace QtMobility;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    qDebug() << "MainWindow::MainWindow(QWidget *parent)";
    ui->setupUi(this);
    this->setWindowTitle("Call Monitor");
    ui->lstRxMsg->setModel(&m_rxDBusMsg);
    telephoycalllist = new QtMobility::QTelephonyCallList(this);
    connect(telephoycalllist, SIGNAL(activeCallAdded(QTelephonyCallInfo)), SLOT(onActiveCallAdded(QTelephonyCallInfo)));
    connect(telephoycalllist, SIGNAL(activeCallStatusChanged(QTelephonyCallInfo)), SLOT(onActiveCallStatusChanged(QTelephonyCallInfo)));
}

MainWindow::~MainWindow()
{
    qDebug() << "MainWindow::~MainWindow()";
    if(telephoycalllist != 0){
        delete telephoycalllist;
    }
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::addListEntry(QString value)
{
    QStringList vl = m_rxDBusMsg.stringList();
    vl.append(value);
    m_rxDBusMsg.setStringList(vl);
    ui->lstRxMsg->scrollToBottom();

}

void MainWindow::onActiveCallAdded(const QTelephonyCallInfo& call)
{
    addListEntry("call Added: " + call.remotePartyIdentifier());
    if(call.type() == QTelephony::Video)
        addListEntry("- type: Video");
    else if(call.type() == QTelephony::Voice)
        addListEntry("- type: Voice");
    else if(call.type() == QTelephony::Text)
        addListEntry("- type: Text");
    else if(call.type() == QTelephony::Any)
        addListEntry("- type: Any");
}

void MainWindow::onActiveCallStatusChanged(const QTelephonyCallInfo& call)
{
    addListEntry("status changed: " + call.remotePartyIdentifier());
    if(call.status() == QTelephony::Idle)
        addListEntry("- Idle");
    else if(call.status() == QTelephony::Dialing)
        addListEntry("- Dialing");
    else if(call.status() == QTelephony::Alerting)
        addListEntry("- Alerting");
    else if(call.status() == QTelephony::Connected)
        addListEntry("- Connected");
    else if(call.status() == QTelephony::Disconnecting)
        addListEntry("- Disconnecting");
    else if(call.status() == QTelephony::Incomming)
        addListEntry("- Incomming");
    else if(call.status() == QTelephony::OnHold)
        addListEntry("- OnHold");
}