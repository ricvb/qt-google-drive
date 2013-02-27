#include "driveengine.h"
#include "share/debug.h"
#include "settings/settingsmanager.h"
#include "gui/controls/spacer.h"
#include <QMessageBox>
#include <QHBoxLayout>
#include <QSplitter>
#include <QDir>
#include <QPushButton>
#include <QPicture>

DriveEngine::DriveEngine(QWidget *p) :
    parent(p)
{
}

DriveEngine::~DriveEngine()
{
}

void DriveEngine::init(void)
{
    SUi::inst()->mainToolBar->addWidget(new Spacer(SUi::inst()->centralWidget));
    SUi::inst()->mainToolBar->addAction(SUi::inst()->actionAbout);
    SUi::inst()->shareButton->setVisible(false);

    QHBoxLayout *hBoxLayout = new QHBoxLayout(SUi::inst()->panelsWidget);
    QSplitter *hSplitter = new QSplitter(Qt::Horizontal, SUi::inst()->panelsWidget);

    filesViews[ELeft] = new FilePanel(ELeft);
    filesViews[ERight] = new FilePanel(ERight);

    hBoxLayout->setContentsMargins(0, 0, 0, 0);
    hBoxLayout->addWidget(hSplitter);

    hSplitter->setHandleWidth(1);

    hSplitter->addWidget(filesViews[ELeft]);
    hSplitter->addWidget(filesViews[ERight]);

    for(int i = 0; i < EPanelsCount; ++i)
    {
        contentMngr[i].reset(new ContentManager);
        contentMngr[i]->init();
    }

    checkUI.reset(new CheckUI);
    filesTransferUI.reset(new FilesTransferUI);
    contentUI.reset(new ContentUI);

    connect(filesViews[ELeft], SIGNAL(signalAccountChanged(int, const QString&)), SLOT(slotAccountChanged(int, const QString&)));
    connect(filesViews[ERight], SIGNAL(signalAccountChanged(int, const QString&)), SLOT(slotAccountChanged(int, const QString&)));

    if(SettingsManager().isAnyAccount()) updatePanel(ELeft, true);
    if(!SettingsManager().isWorkDirSet()) checkUI->slotCheckWorkDir(true);
}

FilePanel* DriveEngine::getFilePanel(int panel) const
{
    return filesViews[panel];
}

ContentManager* DriveEngine::getContentMngr(bool opposite) const
{
    ContentManager* cm;
    EPanels currentPanel = static_cast<EPanels> (SettingsManager().currentPanel());

    if(currentPanel == ELeft)
    {
        if(opposite) cm = contentMngr[ERight].data();
        else cm = contentMngr[ELeft].data();
    }

    if(currentPanel == ERight)
    {
        if(opposite) cm = contentMngr[ELeft].data();
        else cm = contentMngr[ERight].data();
    }

    return cm;
}

void DriveEngine::updatePanel(int panelNum, bool initLoad)
{
    SettingsManager settingsManager;
    EPanels panelId = static_cast <EPanels> (panelNum);
    QString disc;

    settingsManager.setInitialLoading(initLoad);
    settingsManager.setCurrentPanel(panelNum);

    disc = settingsManager.accountDisc(settingsManager.currentAccount(panelNum));
    disc += QString(":");
    disc += QDir::toNativeSeparators("/");

    contentUI->getPanelLabel(panelId)->setText(disc + settingsManager.currentFolderPath(panelNum));
    getContentMngr()->setPathesURLs(settingsManager.pathesURLs(panelNum));

    contentMngr[panelNum]->setPanel(filesViews[panelNum]->getFileView(), filesViews[panelNum]->getpanelNum());
    contentMngr[panelNum]->get(settingsManager.currentFolderURL(panelNum));

    getFilePanel(panelNum)->fillComboBox(settingsManager.accountsWithLetters(), settingsManager.currentAccount(panelNum));
}

void DriveEngine::slotFirstPanelIsLoaded(void)
{
    updatePanel(ERight, false);
}

void DriveEngine::slotAccountChanged(int panelNum, const QString &accountName)
{
    SettingsManager settingsManager;

    if(settingsManager.currentAccount(panelNum) != accountName)
    {
        settingsManager.setCurrentAccount(panelNum, accountName);
        updatePanel(panelNum, false);
    }
}

CheckUI* DriveEngine::getCheckUI(void) const
{
    return checkUI.data();
}

ContentUI* DriveEngine::getContentUI(void) const
{
    return contentUI.data();
}

FilesTransferUI* DriveEngine::getfilesTransferUI(void) const
{
    return filesTransferUI.data();
}

QWidget* DriveEngine::getParent(void) const
{
    return parent;
}
