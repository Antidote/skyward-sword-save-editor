#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "gamefile.h"
#include <QFile>
#include <QString>
#include <QMessageBox>
#include <QFileDialog>
#include <QApplication>

#ifdef DEBUG
QString dir("D:/Projects/dolphin-emu/Binary/x64/User/Wii/title/00010000/534f5545/data");
#else
QString dir("");
#endif

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow),
    m_gameFile(NULL),
    m_isUpdating(false),
    m_curGame(GameFile::GameNone)
{
    m_ui->setupUi(this);

    SetupActions();
    SetupConnections();

    ToggleVisible(false);
}

MainWindow::~MainWindow()
{
    delete m_ui;
    if (m_gameFile != NULL)
    {
        if (m_gameFile->IsOpen())
            m_gameFile->Close();

        delete m_gameFile;
    }
}

void MainWindow::SetupActions()
{
    // File -> Open
    m_ui->actionOpen->setShortcuts(QKeySequence::Open);
    m_ui->actionOpen->setStatusTip(tr("Opens a Skyward Sword Save File..."));
    // File -> Save
    m_ui->actionSave->setShortcuts(QKeySequence::Save);
    m_ui->actionSave->setStatusTip(tr("Saves the current open file..."));
    // File -> Save As
    m_ui->actionSaveAs->setShortcuts(QKeySequence::SaveAs);
    m_ui->actionSaveAs->setStatusTip(tr("Saves the current open file, prompting for a new location..."));
    // File->Close
    m_ui->actionClose->setShortcut(QKeySequence::Close);
    m_ui->actionClose->setStatusTip(tr("Closes the current file..."));
    // File -> Exit
    m_ui->actionExit->setShortcuts(QKeySequence::Quit);
    m_ui->actionExit->setStatusTip(tr("Exits the application..."));
    // Toolbar -> Reload
    m_ui->actionReload->setShortcuts(QKeySequence::Refresh);
    m_ui->actionReload->setStatusTip(tr("Reloads the current file..."));

    m_gameGroup = new QActionGroup(this);
    m_gameGroup->addAction(m_ui->actionGame1);
    m_gameGroup->addAction(m_ui->actionGame2);
    m_gameGroup->addAction(m_ui->actionGame3);
}

void MainWindow::SetupConnections()
{
    connect(m_ui->nameLineEdit,    SIGNAL(textChanged(QString)), this, SLOT(onTextChanged(QString)));
    connect(m_ui->playerXSpinBox,  SIGNAL(valueChanged(double)), this, SLOT(onValueChanged()));
    connect(m_ui->playerYSpinBox,  SIGNAL(valueChanged(double)), this, SLOT(onValueChanged()));
    connect(m_ui->playerZSpinBox,  SIGNAL(valueChanged(double)), this, SLOT(onValueChanged()));
    connect(m_ui->rupeeSpinBox,    SIGNAL(valueChanged(int)),    this, SLOT(onValueChanged()));
    connect(m_ui->totalHPSpinBox,  SIGNAL(valueChanged(int)),    this, SLOT(onValueChanged()));
    connect(m_ui->unkHPSpinBox,    SIGNAL(valueChanged(int)),    this, SLOT(onValueChanged()));
    connect(m_ui->curHPSpinBox,    SIGNAL(valueChanged(int)),    this, SLOT(onValueChanged()));
    connect(m_ui->curMapLineEdit,  SIGNAL(textChanged(QString)), this, SLOT(onTextChanged(QString)));
    connect(m_ui->curAreaLineEdit,  SIGNAL(textChanged(QString)), this, SLOT(onTextChanged(QString)));
    connect(m_ui->curRoomLineEdit,  SIGNAL(textChanged(QString)), this, SLOT(onTextChanged(QString)));
    connect(m_gameGroup,           SIGNAL(triggered(QAction*)),  this, SLOT(onGameChanged(QAction*)));
    connect(m_ui->actionOpen,      SIGNAL(triggered()),          this, SLOT(onOpen()));
    connect(m_ui->actionSave,      SIGNAL(triggered()),          this, SLOT(onSave()));
    connect(m_ui->actionClose,     SIGNAL(triggered()),          this, SLOT(onClose()));
    connect(m_ui->actionReload,    SIGNAL(triggered()),          this, SLOT(onReload()));
    connect(m_ui->actionExit,      SIGNAL(triggered()),          this, SLOT(close()));
}

void MainWindow::onTextChanged(QString text)
{
    if (!m_gameFile || !m_gameFile->IsOpen() ||
         m_isUpdating || m_curGame == GameFile::GameNone)
        return;

    if (m_ui->nameLineEdit->isModified())
    {
        m_ui->nameLineEdit->setModified(false);
        m_gameFile->SetPlayerName(text);
    }

    if (m_ui->curMapLineEdit->isModified())
    {
        m_ui->curMapLineEdit->setModified(false);
        m_gameFile->SetCurrentMap(text);
    }

    if (m_ui->curAreaLineEdit->isModified())
    {
        m_ui->curAreaLineEdit->setModified(false);
        m_gameFile->SetCurrentArea(text);
    }

    if (m_ui->curRoomLineEdit->isModified())
    {
        m_ui->curRoomLineEdit->setModified(false);
        m_gameFile->SetCurrentRoom(text);
    }

    m_gameFile->UpdateChecksum();
    this->setWindowTitle("0x" + QString("").sprintf("%08X", m_gameFile->GetChecksum()));
}

void MainWindow::onValueChanged()
{
    if (!m_gameFile || !m_gameFile->IsOpen() ||
         m_isUpdating || m_curGame == GameFile::GameNone)
        return;
    m_gameFile->SetPlayerX((float)m_ui->playerXSpinBox->value());
    m_gameFile->SetPlayerY((float)m_ui->playerYSpinBox->value());
    m_gameFile->SetPlayerZ((float)m_ui->playerZSpinBox->value());
    m_gameFile->SetTotalHP((short)m_ui->totalHPSpinBox->value());
    m_gameFile->SetUnkHP((short)m_ui->unkHPSpinBox->value());
    m_gameFile->SetCurrentHP((short)m_ui->curHPSpinBox->value());
    m_gameFile->SetRupees((short)m_ui->rupeeSpinBox->value());
}

void MainWindow::onOpen()
{
    QFileDialog fileDialog;
    QString file = fileDialog.getOpenFileName(this, tr("Open Skyward Sword Save..."), dir, tr("Skyward Sword Save Files (*.sav)"));
    if (file.size() > 0)
    {
        if (m_gameFile == NULL)
            m_gameFile = new GameFile();
        else
            m_gameFile->Close();

        if (m_gameFile->Open(m_curGame, file))
            UpdateInfo();
    }
}

void MainWindow::onSave()
{
    if (!m_gameFile || !m_gameFile->IsOpen())
        return;

    if(m_gameFile->Save())
        m_ui->statusBar->showMessage(tr("Save successful!"));
    else
        m_ui->statusBar->showMessage(tr("Unable to save file"));
}

void MainWindow::onSaveAs()
{
}

void MainWindow::onGameChanged(QAction* game)
{
    if (!m_gameFile)
        return;

    if (game == m_ui->actionGame1)
        m_curGame = GameFile::Game1;
    else if (game == m_ui->actionGame2)
        m_curGame = GameFile::Game2;
    else if (game == m_ui->actionGame3)
        m_curGame = GameFile::Game3;

    m_gameFile->SetGame(m_curGame);
    UpdateInfo();
}

void MainWindow::onReload()
{
    if (!m_gameFile || !m_gameFile->IsOpen())
        return;

    m_gameFile->Reload(m_curGame);
    if(m_gameFile->IsOpen())
    {
        UpdateInfo();
        m_ui->statusBar->showMessage(tr("File successfully reloaded"));
    }
    else
    {
        ClearInfo();
        m_ui->statusBar->showMessage(tr("Unable to reload file, is it still there?"));
    }
}

void MainWindow::onClose()
{
    if (!m_gameFile || !m_gameFile->IsOpen())
        return;
    m_gameFile->Close();
    delete m_gameFile;
    m_gameFile = NULL;

    ClearInfo();
    ToggleVisible(false);
}

void MainWindow::UpdateInfo()
{
    if (!m_gameFile || !m_gameFile->IsOpen() ||
         m_isUpdating || m_curGame == GameFile::GameNone)
    {
        ToggleVisible(false);
        return;
    }


    m_isUpdating = true;
    m_ui->playerXSpinBox->setValue(m_gameFile->GetPlayerX());
    m_ui->playerYSpinBox->setValue(m_gameFile->GetPlayerY());
    m_ui->playerZSpinBox->setValue(m_gameFile->GetPlayerZ());
    m_ui->curMapLineEdit->setText(m_gameFile->GetCurrentMap());
    m_ui->curAreaLineEdit->setText(m_gameFile->GetCurrentArea());
    m_ui->curRoomLineEdit->setText(m_gameFile->GetCurrentRoom());
    m_ui->nameLineEdit->setText(m_gameFile->GetPlayerName());
    m_ui->rupeeSpinBox->setValue(m_gameFile->GetRupees());
    m_ui->totalHPSpinBox->setValue(m_gameFile->GetTotalHP());
    m_ui->unkHPSpinBox->setValue(m_gameFile->GetUnkHP());
    m_ui->curHPSpinBox->setValue(m_gameFile->GetCurrentHP());
    m_isUpdating = false;

    ToggleVisible(true);
}

void MainWindow::ClearInfo()
{
    m_isUpdating = true;
    m_ui->nameLineEdit->clear();
    m_ui->rupeeSpinBox->clear();
    m_ui->totalHPSpinBox->clear();
    m_ui->unkHPSpinBox->clear();
    m_ui->curHPSpinBox->clear();
    m_isUpdating = false;
}

void MainWindow::ToggleVisible(bool visible)
{
    m_ui->tabWidget->setVisible(visible);
}
