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
    m_isUpdating(false)
{
    m_ui->setupUi(this);

    SetupActions();
    SetupConnections();

    m_checkTimer = new QTimer(this);
    connect(m_checkTimer, SIGNAL(timeout()), this, SLOT(onCheck()));
    m_checkTimer->start(UPDATE_DELAY); // set check for ever 5 seconds

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

// TODO: Need to make this more intelligent.
void MainWindow::onCheck()
{
    if (m_gameFile && m_gameFile->IsOpen() && m_gameFile->HasFileChanged())
    {
        QMessageBox msg(QMessageBox::Question, "File Modified", "The file on disk has been modified outside of the editor, reload?", QMessageBox::Ok | QMessageBox::Cancel);
        int ret = msg.exec();
        if (ret == QMessageBox::Ok)
        {
            m_gameFile->Reload(m_gameFile->GetGame());
            UpdateInfo();
        }
    }
    m_checkTimer->start(UPDATE_DELAY);
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
    connect(m_ui->playHoursSpinBox,   SIGNAL(valueChanged(int)),    this, SLOT(onValueChanged()));
    connect(m_ui->playMinutesSpinBox, SIGNAL(valueChanged(int)),    this, SLOT(onValueChanged()));
    connect(m_ui->playSecondsSpinBox, SIGNAL(valueChanged(int)),    this, SLOT(onValueChanged()));
    connect(m_ui->saveTimeEdit,       SIGNAL(dateTimeChanged(QDateTime)),   this, SLOT(onDateTimeChanged(QDateTime)));
    connect(m_ui->playerXSpinBox,     SIGNAL(valueChanged(double)), this, SLOT(onValueChanged()));
    connect(m_ui->playerYSpinBox,     SIGNAL(valueChanged(double)), this, SLOT(onValueChanged()));
    connect(m_ui->playerZSpinBox,     SIGNAL(valueChanged(double)), this, SLOT(onValueChanged()));
    connect(m_ui->playerRollSpinBox,  SIGNAL(valueChanged(double)), this, SLOT(onValueChanged()));
    connect(m_ui->playerPitchSpinBox, SIGNAL(valueChanged(double)), this, SLOT(onValueChanged()));
    connect(m_ui->playerYawSpinBox,   SIGNAL(valueChanged(double)), this, SLOT(onValueChanged()));
    connect(m_ui->cameraXSpinBox,     SIGNAL(valueChanged(double)), this, SLOT(onValueChanged()));
    connect(m_ui->cameraYSpinBox,     SIGNAL(valueChanged(double)), this, SLOT(onValueChanged()));
    connect(m_ui->cameraZSpinBox,     SIGNAL(valueChanged(double)), this, SLOT(onValueChanged()));
    connect(m_ui->cameraRollSpinBox,  SIGNAL(valueChanged(double)), this, SLOT(onValueChanged()));
    connect(m_ui->cameraPitchSpinBox, SIGNAL(valueChanged(double)), this, SLOT(onValueChanged()));
    connect(m_ui->cameraYawSpinBox,   SIGNAL(valueChanged(double)), this, SLOT(onValueChanged()));
    connect(m_ui->nameLineEdit,       SIGNAL(textChanged(QString)), this, SLOT(onTextChanged(QString)));
    connect(m_ui->trainingSwdChkBox,  SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->bugNetChkBox,       SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->grassHopperChkBox,  SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->rhinoBeetleChkBox,  SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->mantisChkBox,       SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->ladybugChkBox,      SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->butterflyChkBox,    SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->rupeeSpinBox,       SIGNAL(valueChanged(int)),    this, SLOT(onValueChanged()));
    connect(m_ui->totalHPSpinBox,     SIGNAL(valueChanged(int)),    this, SLOT(onValueChanged()));
    connect(m_ui->unkHPSpinBox,       SIGNAL(valueChanged(int)),    this, SLOT(onValueChanged()));
    connect(m_ui->curHPSpinBox,       SIGNAL(valueChanged(int)),    this, SLOT(onValueChanged()));
    connect(m_ui->curMapLineEdit,     SIGNAL(textChanged(QString)), this, SLOT(onTextChanged(QString)));
    connect(m_ui->curAreaLineEdit,    SIGNAL(textChanged(QString)), this, SLOT(onTextChanged(QString)));
    connect(m_ui->curRoomLineEdit,    SIGNAL(textChanged(QString)), this, SLOT(onTextChanged(QString)));
    connect(m_gameGroup,              SIGNAL(triggered(QAction*)),  this, SLOT(onGameChanged(QAction*)));
    connect(m_ui->actionOpen,         SIGNAL(triggered()),          this, SLOT(onOpen()));
    connect(m_ui->actionSave,         SIGNAL(triggered()),          this, SLOT(onSave()));
    connect(m_ui->actionClose,        SIGNAL(triggered()),          this, SLOT(onClose()));
    connect(m_ui->actionReload,       SIGNAL(triggered()),          this, SLOT(onReload()));
    connect(m_ui->actionExit,         SIGNAL(triggered()),          this, SLOT(close()));
}

void MainWindow::onTextChanged(QString text)
{
    if (!m_gameFile || !m_gameFile->IsOpen() ||
         m_isUpdating || m_gameFile->GetGame() == GameFile::GameNone)
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
    UpdateTitle();
}

void MainWindow::onValueChanged()
{
    if (!m_gameFile || !m_gameFile->IsOpen() ||
         m_isUpdating || m_gameFile->GetGame() == GameFile::GameNone)
        return;
    PlayTime playTime;
    playTime.Hours = m_ui->playHoursSpinBox->value();
    playTime.Minutes = m_ui->playMinutesSpinBox->value();
    playTime.Seconds = m_ui->playSecondsSpinBox->value();
    playTime.RawTicks = (((playTime.Hours * 60) * 60) + (playTime.Minutes * 60) + playTime.Seconds) * TICKS_PER_SECOND;
    m_gameFile->SetPlayTime(playTime);
    m_gameFile->SetPlayerX((float)m_ui->playerXSpinBox->value());
    m_gameFile->SetPlayerY((float)m_ui->playerYSpinBox->value());
    m_gameFile->SetPlayerZ((float)m_ui->playerZSpinBox->value());
    m_gameFile->SetPlayerRoll((float)m_ui->playerRollSpinBox->value());
    m_gameFile->SetPlayerPitch((float)m_ui->playerPitchSpinBox->value());
    m_gameFile->SetPlayerYaw((float)m_ui->playerYawSpinBox->value());
    m_gameFile->SetCameraX((float)m_ui->cameraXSpinBox->value());
    m_gameFile->SetCameraY((float)m_ui->cameraYSpinBox->value());
    m_gameFile->SetCameraZ((float)m_ui->cameraZSpinBox->value());
    m_gameFile->SetCameraRoll((float)m_ui->cameraRollSpinBox->value());
    m_gameFile->SetCameraPitch((float)m_ui->cameraPitchSpinBox->value());
    m_gameFile->SetCameraYaw((float)m_ui->cameraYawSpinBox->value());
    m_gameFile->SetTotalHP((short)m_ui->totalHPSpinBox->value());
    m_gameFile->SetUnkHP((short)m_ui->unkHPSpinBox->value());
    m_gameFile->SetCurrentHP((short)m_ui->curHPSpinBox->value());
    m_gameFile->SetTrainingSword(m_ui->trainingSwdChkBox->isChecked());
    m_gameFile->SetBugNet(m_ui->bugNetChkBox->isChecked());
    m_gameFile->SetFaronGrasshopper(m_ui->grassHopperChkBox->isChecked());
    m_gameFile->SetWoodlandRhinoBeetle(m_ui->rhinoBeetleChkBox->isChecked());
    m_gameFile->SetSkyloftMantis(m_ui->mantisChkBox->isChecked());
    m_gameFile->SetVolcanicLadybug(m_ui->ladybugChkBox->isChecked());
    m_gameFile->SetBlessedButterfly(m_ui->butterflyChkBox->isChecked());
    m_gameFile->SetRupees((short)m_ui->rupeeSpinBox->value());

    m_gameFile->UpdateChecksum();
    UpdateTitle();
}

void MainWindow::onDateTimeChanged(QDateTime val)
{
    if (!m_gameFile || !m_gameFile->IsOpen() ||
         m_isUpdating || m_gameFile->GetGame() == GameFile::GameNone)
        return;

    m_gameFile->SetSaveTime(val);
    m_gameFile->UpdateChecksum();
    UpdateTitle();
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

        if (m_gameFile->Open(m_gameFile->GetGame(), file))
        {
            if (!m_gameFile->HasValidChecksum())
            {
                QMessageBox msg(QMessageBox::Warning, "CRC32 Mismatch", "The checksum generated does not match the one provided by the file");
                msg.exec();
            }
            UpdateInfo();
        }
    }
}

void MainWindow::onSave()
{
    if (!m_gameFile || !m_gameFile->IsOpen() || m_gameFile->GetGame() == GameFile::GameNone)
        return;

    if(m_gameFile->Save())
        m_ui->statusBar->showMessage(tr("Save successful!"));
    else
        m_ui->statusBar->showMessage(tr("Unable to save file"));

    m_gameFile->UpdateChecksum();
    UpdateTitle();
}

void MainWindow::onSaveAs()
{
}

void MainWindow::onGameChanged(QAction* game)
{
    if (!m_gameFile || m_isUpdating)
        return;

    quint32 curGame;

    if (game == m_ui->actionGame1)
        curGame = GameFile::Game1;
    else if (game == m_ui->actionGame2)
        curGame = GameFile::Game2;
    else if (game == m_ui->actionGame3)
        curGame = GameFile::Game3;

    m_gameFile->SetGame((GameFile::Game)curGame);
    UpdateInfo();
    UpdateTitle();
}

void MainWindow::onReload()
{
    if (!m_gameFile || !m_gameFile->IsOpen())
        return;

    m_gameFile->Reload(m_gameFile->GetGame());
    if(m_gameFile->IsOpen())
    {
        UpdateInfo();
        m_ui->statusBar->showMessage(tr("File successfully reloaded"));
        UpdateTitle();
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
         m_isUpdating || m_gameFile->GetGame() == GameFile::GameNone)
    {
        ToggleVisible(false);
        return;
    }

    m_isUpdating = true;
    m_ui->playHoursSpinBox->setValue(m_gameFile->GetPlayTime().Hours);
    m_ui->playMinutesSpinBox->setValue(m_gameFile->GetPlayTime().Minutes);
    m_ui->playSecondsSpinBox->setValue(m_gameFile->GetPlayTime().Seconds);
    m_gameFile->SetPlayTime(m_gameFile->GetPlayTime());
    m_ui->saveTimeEdit->setDateTime(m_gameFile->GetSaveTime());
    m_ui->playerXSpinBox->setValue(m_gameFile->GetPlayerX());
    m_ui->playerYSpinBox->setValue(m_gameFile->GetPlayerY());
    m_ui->playerZSpinBox->setValue(m_gameFile->GetPlayerZ());
    m_ui->playerRollSpinBox->setValue(m_gameFile->GetPlayerRoll());
    m_ui->playerPitchSpinBox->setValue(m_gameFile->GetPlayerPitch());
    m_ui->playerYawSpinBox->setValue(m_gameFile->GetPlayerYaw());
    m_ui->cameraXSpinBox->setValue(m_gameFile->GetCameraX());
    m_ui->cameraYSpinBox->setValue(m_gameFile->GetCameraY());
    m_ui->cameraZSpinBox->setValue(m_gameFile->GetCameraZ());
    m_ui->cameraRollSpinBox->setValue(m_gameFile->GetCameraRoll());
    m_ui->cameraPitchSpinBox->setValue(m_gameFile->GetCameraPitch());
    m_ui->cameraYawSpinBox->setValue(m_gameFile->GetCameraYaw());
    m_ui->curMapLineEdit->setText(m_gameFile->GetCurrentMap());
    m_ui->curAreaLineEdit->setText(m_gameFile->GetCurrentArea());
    m_ui->curRoomLineEdit->setText(m_gameFile->GetCurrentRoom());
    m_ui->nameLineEdit->setText(m_gameFile->GetPlayerName());
    m_ui->trainingSwdChkBox->setChecked(m_gameFile->GetTrainingSword());
    m_ui->bugNetChkBox->setChecked(m_gameFile->GetBugNet());
    m_ui->grassHopperChkBox->setChecked(m_gameFile->GetFaronGrasshopper());
    m_ui->rhinoBeetleChkBox->setChecked(m_gameFile->GetWoodlandRhinoBeetle());
    m_ui->mantisChkBox->setChecked(m_gameFile->GetSkyloftMantis());
    m_ui->ladybugChkBox->setChecked(m_gameFile->GetVolcanicLadybug());
    m_ui->butterflyChkBox->setChecked(m_gameFile->GetBlessedButterfly());
    m_ui->rupeeSpinBox->setValue(m_gameFile->GetRupees());
    m_ui->totalHPSpinBox->setValue(m_gameFile->GetTotalHP());
    m_ui->unkHPSpinBox->setValue(m_gameFile->GetUnkHP());
    m_ui->curHPSpinBox->setValue(m_gameFile->GetCurrentHP());
    m_isUpdating = false;

    ToggleVisible(true);
}

void MainWindow::UpdateTitle()
{
    if (m_gameFile == NULL || !m_gameFile->IsOpen() || m_gameFile->GetGame() == GameFile::GameNone)
        this->setWindowTitle("WiiKing2 Editor");
    else
        this->setWindowTitle(QString("WiiKing2 Editor - Game %1 0x")
                             .arg(m_gameFile->GetGame() + 1)
                             .append(QString("").sprintf("%08X", m_gameFile->GetChecksum())));
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
