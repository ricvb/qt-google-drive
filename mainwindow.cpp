#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFile>
#include "qt_windows.h"

Ui::MainWindow* UiInstance::ui = NULL;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    driveEngine(NULL)
{
    UiInstance::Instance()->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete UiInstance::ui;
    if(driveEngine) delete driveEngine;
}

void MainWindow::init(void)
{
    if(driveEngine) delete driveEngine;

    driveEngine = new DriveEngine(this);
    driveEngine->init();
    driveEngine->slotCheckWorkDir(false);

    setConnections();

    QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

    emit siganalGet();
}

void MainWindow::setConnections(void)
{
    connect(UiInstance::ui->actionMenuLogin, SIGNAL(triggered()), driveEngine, SLOT(slotStartLogin()));
    connect(UiInstance::ui->actionMenuQuit, SIGNAL(triggered()), this, SLOT(close()));
    connect(UiInstance::ui->actionMenuDownload, SIGNAL(triggered()), driveEngine, SLOT(slotDownload()));
    connect(UiInstance::ui->actionDownload, SIGNAL(triggered()), driveEngine, SLOT(slotDownload()));
    connect(UiInstance::ui->actionMenuUpload, SIGNAL(triggered()), driveEngine, SLOT(slotUpload()));
    connect(UiInstance::ui->actionUpload, SIGNAL(triggered()), driveEngine, SLOT(slotUpload()));
    connect(UiInstance::ui->actionMenuSettings, SIGNAL(triggered()), driveEngine, SLOT(slotCheckWorkDir()));
    connect(UiInstance::ui->actionSettings, SIGNAL(triggered()), driveEngine, SLOT(slotCheckWorkDir()));
    connect(driveEngine->getOAuth2(), SIGNAL(loginDone()), this, SLOT(slotloginDone()));
    connect(driveEngine, SIGNAL(signalAccessTokenExpired()), driveEngine, SLOT(slotStartLogin()));
}

Ui::MainWindow* UiInstance::Instance()
{
    if(!UiInstance::ui)
        UiInstance::ui = new Ui::MainWindow;

    return UiInstance::ui;
}

void MainWindow::slotloginDone(void)
{
    init();
}

void MainWindow::slotUpdateModel()
{
  qDebug() << "slotUpdateModel()";
  Sleep(1000);
  init();
}

