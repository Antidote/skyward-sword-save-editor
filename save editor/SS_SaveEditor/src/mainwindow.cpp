// This file is part of WiiKing2 Editor.
//
// WiiKing2 Editor is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Wiiking2 Editor is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with WiiKing2 Editor.  If not, see <http://www.gnu.org/licenses/>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QString>
#include <QMessageBox>
#include <QFileDialog>
#include <QApplication>
#include <QDropEvent>
#include <QUrl>
#include <QDebug>

#include "igamefile.h"
#include "skywardswordfile.h"
#include "newgamedialog.h"
#include "aboutdialog.h"
#include "fileinfodialog.h"

#ifdef DEBUG
QString dir("D:/Projects/dolphin-emu/Binary/x64/User/Wii/title/00010000/534f5545/data");
#else
QString dir("");
#endif

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow),
    m_gameFile(NULL),
    m_curGame(IGameFile::Game1),
    m_isUpdating(false)
{
    m_ui->setupUi(this);

    setWindowFlags(Qt::Window | Qt::WindowMinimizeButtonHint);
    SetupActions();
    SetupConnections();


   //m_checkTimer = new QTimer(this);
   // connect(m_checkTimer, SIGNAL(timeout()), this, SLOT(onCheck()));
   // m_checkTimer->start(UPDATE_DELAY); // set check for ever 5 seconds
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

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    SkywardSwordFile::Region region;
    if (event->mimeData()->urls().count() == 1 && SkywardSwordFile::IsValidFile(event->mimeData()->urls()[0].toLocalFile(), &region))
    {
        event->acceptProposedAction();
        statusBar()->showMessage(QString("File Valid (%1)").arg((region == SkywardSwordFile::NTSCURegion ? "NTSC-U" : region == SkywardSwordFile::NTSCJRegion ? "NTSC-J" : "PAL")));
    }
}

void MainWindow::dragLeaveEvent(QDragLeaveEvent *event)
{
    Q_UNUSED(event)

    statusBar()->clearMessage();
}

void MainWindow::dropEvent(QDropEvent* event)
{
    const QMimeData* mimeData = event->mimeData();

    // check for our needed mime type, here a file or a list of files
    if (mimeData->hasUrls())
    {
        if (!m_gameFile)
            m_gameFile = new SkywardSwordFile();

        if (m_curGame == SkywardSwordFile::GameNone)
            m_curGame = SkywardSwordFile::Game1;


        if (m_gameFile->Open(m_curGame, mimeData->urls()[0].toLocalFile()))
        {
            UpdateInfo();
            UpdateTitle();
        }
    }
    statusBar()->clearMessage();
}
/*
bool MainWindow::event(QEvent *event)
{
    if (event->type() == QEvent::Enter)
        onCheck();

    return QMainWindow::event(event);
}

// TODO: Need to make this more intelligent.
void MainWindow::onCheck()
{
    if (m_gameFile && m_gameFile->IsOpen() && m_gameFile->HasFileOnDiskChanged())
    {
        QMessageBox msg(QMessageBox::Question, "File Modified", "The file on disk has been modified outside of the editor, reload?", QMessageBox::Ok | QMessageBox::Cancel, this);
        int ret = msg.exec();
        if (ret == QMessageBox::Ok)
        {
            m_gameFile->Reload(m_gameFile->GetGame());
            UpdateInfo();
        }
    }
    //m_checkTimer->start(UPDATE_DELAY);
}
*/
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
    connect(m_ui->heroModeChkBox,     SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->introViewedChkBox,  SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->nameLineEdit,       SIGNAL(textChanged(QString)), this, SLOT(onTextChanged(QString)));
    connect(m_ui->curMapLineEdit,     SIGNAL(textChanged(QString)), this, SLOT(onTextChanged(QString)));
    connect(m_ui->curAreaLineEdit,    SIGNAL(textChanged(QString)), this, SLOT(onTextChanged(QString)));
    connect(m_ui->curRoomLineEdit,    SIGNAL(textChanged(QString)), this, SLOT(onTextChanged(QString)));
    connect(m_gameGroup,              SIGNAL(triggered(QAction*)),  this, SLOT(onGameChanged(QAction*)));
    connect(m_ui->actionOpen,         SIGNAL(triggered()),          this, SLOT(onOpen()));
    connect(m_ui->createDeleteGameBtn,SIGNAL(clicked()),            this, SLOT(onCreateNewGame()));
    connect(m_ui->actionSave,         SIGNAL(triggered()),          this, SLOT(onSave()));
    connect(m_ui->actionSaveAs,       SIGNAL(triggered()),          this, SLOT(onSaveAs()));
    connect(m_ui->actionClose,        SIGNAL(triggered()),          this, SLOT(onClose()));
    connect(m_ui->actionReload,       SIGNAL(triggered()),          this, SLOT(onReload()));
    connect(m_ui->actionExit,         SIGNAL(triggered()),          this, SLOT(close()));
    connect(m_ui->actionAbout,        SIGNAL(triggered()),          this, SLOT(onAbout()));
    connect(m_ui->actionAboutQt,      SIGNAL(triggered()),          this, SLOT(onAboutQt()));
    connect(m_ui->actionFileInfo,     SIGNAL(triggered()),          this, SLOT(onFileInfo()));

    // Swords
    connect(m_ui->practiceSwdChkBox,  SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->goddessSwdChkBox,   SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->longSwdChkBox,      SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->whiteSwdChkBox,     SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->masterSwdChkBox,    SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->trueMasterSwdChkBox,SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    // Weapons
    connect(m_ui->slingShotChkBox,    SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->scatterShotChkBox,  SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->bugNetChkBox,       SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->bigBugNetChkBox,    SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->beetleChkBox,       SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->hookBeetleChkBox,   SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->quickBeetleChkBox,  SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->toughBeetleChkBox,  SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->bombChkBox,         SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->gustBellowsChkBox,  SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->whipChkBox,         SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->clawShotChkBox,     SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->bowChkBox,          SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->ironBowChkBox,      SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->sacredBowChkBox,    SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->harpChkBox,         SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    // Bugs
    connect(m_ui->grassHopperChkBox,  SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->rhinoBeetleChkBox,  SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->mantisChkBox,       SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->ladybugChkBox,      SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->butterflyChkBox,    SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->rupeeSpinBox,       SIGNAL(valueChanged(int)),    this, SLOT(onValueChanged()));
    connect(m_ui->totalHPSpinBox,     SIGNAL(valueChanged(int)),    this, SLOT(onValueChanged()));
    connect(m_ui->unkHPSpinBox,       SIGNAL(valueChanged(int)),    this, SLOT(onValueChanged()));
    connect(m_ui->curHPSpinBox,       SIGNAL(valueChanged(int)),    this, SLOT(onValueChanged()));
    connect(m_ui->roomIDSpinBox,      SIGNAL(valueChanged(int)),    this, SLOT(onValueChanged()));
}

SkywardSwordFile* MainWindow::GetGameFile()
{
    return m_gameFile;
}

void MainWindow::onTextChanged(QString text)
{
    if (!m_gameFile || !m_gameFile->IsOpen() ||
         m_isUpdating || m_gameFile->GetGame() == SkywardSwordFile::GameNone)
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
         m_isUpdating || m_gameFile->GetGame() == SkywardSwordFile::GameNone)
        return;
    PlayTime playTime;
    playTime.Hours = m_ui->playHoursSpinBox->value();
    playTime.Minutes = m_ui->playMinutesSpinBox->value();
    playTime.Seconds = m_ui->playSecondsSpinBox->value();
    playTime.RawTicks = (((playTime.Hours * 60) * 60) + (playTime.Minutes * 60) + playTime.Seconds) * TICKS_PER_SECOND;
    m_gameFile->SetPlayTime(playTime);
    m_gameFile->SetPlayerPosition((float)m_ui->playerXSpinBox->value(),    (float)m_ui->playerYSpinBox->value(),     (float)m_ui->playerZSpinBox->value());
    m_gameFile->SetPlayerRotation((float)m_ui->playerRollSpinBox->value(), (float)m_ui->playerPitchSpinBox->value(), (float)m_ui->playerYawSpinBox->value() );
    m_gameFile->SetCameraPosition((float)m_ui->cameraXSpinBox->value(),    (float)m_ui->cameraYSpinBox->value(),     (float)m_ui->cameraZSpinBox->value());
    m_gameFile->SetCameraRotation((float)m_ui->cameraRollSpinBox->value(), (float)m_ui->cameraPitchSpinBox->value(), (float)m_ui->cameraYawSpinBox->value());
    m_gameFile->SetHeroMode(m_ui->heroModeChkBox->isChecked());
    m_gameFile->SetIntroViewed(m_ui->introViewedChkBox->isChecked());
    m_gameFile->SetTotalHP((short)m_ui->totalHPSpinBox->value());
    m_gameFile->SetUnkHP((short)m_ui->unkHPSpinBox->value());
    m_gameFile->SetCurrentHP((short)m_ui->curHPSpinBox->value());
    m_gameFile->SetRoomID((uint)m_ui->roomIDSpinBox->value());
    m_gameFile->SetRupees((short)m_ui->rupeeSpinBox->value());
    // Swords
    m_gameFile->SetSword(SkywardSwordFile::PracticeSword,m_ui->practiceSwdChkBox->isChecked());
    m_gameFile->SetSword(SkywardSwordFile::GoddessSword, m_ui->goddessSwdChkBox->isChecked());
    m_gameFile->SetSword(SkywardSwordFile::LongSword, m_ui->longSwdChkBox->isChecked());
    m_gameFile->SetSword(SkywardSwordFile::WhiteSword, m_ui->whiteSwdChkBox->isChecked());
    m_gameFile->SetSword(SkywardSwordFile::MasterSword, m_ui->masterSwdChkBox->isChecked());
    m_gameFile->SetSword(SkywardSwordFile::TrueMasterSword, m_ui->trueMasterSwdChkBox->isChecked());
    // Weapons
    m_gameFile->SetEquipment(SkywardSwordFile::SlingshotWeapon, m_ui->slingShotChkBox->isChecked());
    m_gameFile->SetEquipment(SkywardSwordFile::ScattershotWeapon, m_ui->scatterShotChkBox->isChecked());
    m_gameFile->SetEquipment(SkywardSwordFile::BugnetWeapon, m_ui->bugNetChkBox->isChecked());
    m_gameFile->SetEquipment(SkywardSwordFile::BigBugnetWeapon, m_ui->bigBugNetChkBox->isChecked());
    m_gameFile->SetEquipment(SkywardSwordFile::BeetleWeapon, m_ui->beetleChkBox->isChecked());
    m_gameFile->SetEquipment(SkywardSwordFile::HookBeetleWeapon, m_ui->hookBeetleChkBox->isChecked());
    m_gameFile->SetEquipment(SkywardSwordFile::QuickBeetleWeapon, m_ui->quickBeetleChkBox->isChecked());
    m_gameFile->SetEquipment(SkywardSwordFile::ToughBeetleWeapon, m_ui->toughBeetleChkBox->isChecked());
    m_gameFile->SetEquipment(SkywardSwordFile::BombWeapon, m_ui->bombChkBox->isChecked());
    m_gameFile->SetEquipment(SkywardSwordFile::GustBellowsWeapon, m_ui->gustBellowsChkBox->isChecked());
    m_gameFile->SetEquipment(SkywardSwordFile::WhipWeapon, m_ui->whipChkBox->isChecked());
    m_gameFile->SetEquipment(SkywardSwordFile::ClawshotWeapon, m_ui->clawShotChkBox->isChecked());
    m_gameFile->SetEquipment(SkywardSwordFile::BowWeapon, m_ui->bowChkBox->isChecked());
    m_gameFile->SetEquipment(SkywardSwordFile::IronBowWeapon, m_ui->ironBowChkBox->isChecked());
    m_gameFile->SetEquipment(SkywardSwordFile::SacredBowWeapon, m_ui->sacredBowChkBox->isChecked());
    m_gameFile->SetEquipment(SkywardSwordFile::HarpEquipment, m_ui->harpChkBox->isChecked());
    // Bugs
    m_gameFile->SetBug(SkywardSwordFile::GrasshopperBug, m_ui->grassHopperChkBox->isChecked());
    m_gameFile->SetBug(SkywardSwordFile::RhinoBeetleBug, m_ui->rhinoBeetleChkBox->isChecked());
    m_gameFile->SetBug(SkywardSwordFile::MantisBug, m_ui->mantisChkBox->isChecked());
    m_gameFile->SetBug(SkywardSwordFile::LadybugBug, m_ui->ladybugChkBox->isChecked());
    m_gameFile->SetBug(SkywardSwordFile::ButterflyBug, m_ui->butterflyChkBox->isChecked());
    m_gameFile->UpdateChecksum();
    UpdateTitle();
}

void MainWindow::onDateTimeChanged(QDateTime val)
{
    if (!m_gameFile || !m_gameFile->IsOpen() ||
         m_isUpdating || m_gameFile->GetGame() == SkywardSwordFile::GameNone)
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
            m_gameFile = new SkywardSwordFile();
        else
            m_gameFile->Close();

        if (m_gameFile->Open(m_gameFile->GetGame(), file))
        {
            if (!m_gameFile->HasValidChecksum())
            {
                QMessageBox msg(QMessageBox::Warning, tr("CRC32 Mismatch"), tr("The checksum generated does not match the one provided by the file"));
                msg.exec();
            }
            m_gameFile->SetGame(SkywardSwordFile::Game1);
            m_ui->actionGame1->setChecked(true);
            UpdateInfo();
            UpdateTitle();
        }
    }
}

void MainWindow::onCreateNewGame()
{
    NewGameDialog* ngd = new NewGameDialog(this, m_curGame);
    ngd->setWindowTitle("New Adventure...");
    ngd->exec();
    if (ngd->result() == NewGameDialog::Accepted)
    {
        SkywardSwordFile* tmpFile = ngd->gameFile(m_gameFile);
        m_gameFile = tmpFile;
        UpdateInfo();
        UpdateTitle();
    }
    delete ngd;
}

void MainWindow::onDeleteGame()
{
    if (!m_gameFile || !m_gameFile->IsOpen())
        return;

    m_gameFile->DeleteGame(m_curGame);
    UpdateInfo();
    UpdateTitle();
}

void MainWindow::onSave()
{
    if (!m_gameFile || !m_gameFile->IsOpen() || m_gameFile->GetGame() == SkywardSwordFile::GameNone)
        return;

    if (m_gameFile->GetFilename().size() <= 0)
    {
        QFileDialog fileDialog;
        QString file = fileDialog.getSaveFileName(this, tr("Save Skyward Sword Save File..."), dir, tr("Skyward Sword Save Files (*.sav)"));
        m_gameFile->SetFilename(file);
    }

    if(m_gameFile->Save())
        m_ui->statusBar->showMessage(tr("Save successful!"));
    else
        m_ui->statusBar->showMessage(tr("Unable to save file"));

    m_gameFile->UpdateChecksum();
    UpdateTitle();
}

void MainWindow::onSaveAs()
{
    if (!m_gameFile)
        return;

    m_gameFile->SetFilename(QString(tr("")));
    onSave();
}

void MainWindow::onAbout()
{
    AboutDialog* abt = new AboutDialog(this);
    abt->exec();
}

void MainWindow::onAboutQt()
{
    QApplication::aboutQt();
}

void MainWindow::onFileInfo()
{
    FileInfoDialog* fid = new FileInfoDialog(this, *m_gameFile);
    fid->exec();
}

void MainWindow::onGameChanged(QAction* game)
{
    if (!m_gameFile || m_isUpdating)
        return;

    if (game == m_ui->actionGame1)
        m_curGame = SkywardSwordFile::Game1;
    else if (game == m_ui->actionGame2)
        m_curGame = SkywardSwordFile::Game2;
    else if (game == m_ui->actionGame3)
        m_curGame = SkywardSwordFile::Game3;

    m_gameFile->SetGame((SkywardSwordFile::Game)m_curGame);
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
    if(m_gameFile->IsModified())
    {
        QString filename = QFileInfo(m_gameFile->GetFilename()).fileName();
        QMessageBox msg(QMessageBox::Information,
                        "File Modified",
                        QString("The file \"%1\" has been modified.\n Do you wish to save?")
                        .arg(filename.isEmpty() ? tr("Untitled") : filename),
                        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        int result = msg.exec();
        if (result == QMessageBox::Yes)
            onSave();

        if(result == QMessageBox::Cancel)
            return;
    }


    m_gameFile->Close();
    delete m_gameFile;
    m_gameFile = NULL;

    ClearInfo();
    m_ui->tabWidget->setEnabled(false);

    m_ui->createDeleteGameBtn->setText(tr("Click to create new Adventure"));
    if (m_ui->createDeleteGameBtn->disconnect())
        connect(m_ui->createDeleteGameBtn, SIGNAL(clicked()), this, SLOT(onCreateNewGame()));
    UpdateTitle();
}

void MainWindow::UpdateInfo()
{
    if (!m_gameFile || !m_gameFile->IsOpen() ||
         m_isUpdating || m_gameFile->GetGame() == SkywardSwordFile::GameNone)
        return;

    if (!m_gameFile->IsNew())
    {
        m_ui->createDeleteGameBtn->setText(tr("Delete Adventure"));
        if (m_ui->createDeleteGameBtn->disconnect())
            connect(m_ui->createDeleteGameBtn, SIGNAL(clicked()), this, SLOT(onDeleteGame()));

        m_ui->tabWidget->setEnabled(true);
    }
    else
    {
        m_ui->createDeleteGameBtn->setText(tr("Click to create new Adventure"));
        if (m_ui->createDeleteGameBtn->disconnect())
            connect(m_ui->createDeleteGameBtn, SIGNAL(clicked()), this, SLOT(onCreateNewGame()));

        m_ui->tabWidget->setEnabled(false);
    }

    m_isUpdating = true;
    // Player Stats
    m_ui->nameLineEdit       ->setText(m_gameFile->GetPlayerName());
    m_ui->rupeeSpinBox       ->setValue(m_gameFile->GetRupees());
    m_ui->totalHPSpinBox     ->setValue(m_gameFile->GetTotalHP());
    m_ui->unkHPSpinBox       ->setValue(m_gameFile->GetUnkHP());
    m_ui->curHPSpinBox       ->setValue(m_gameFile->GetCurrentHP());
    m_ui->playHoursSpinBox   ->setValue(m_gameFile->GetPlayTime().Hours);
    m_ui->playMinutesSpinBox ->setValue(m_gameFile->GetPlayTime().Minutes);
    m_ui->playSecondsSpinBox ->setValue(m_gameFile->GetPlayTime().Seconds);
    m_ui->saveTimeEdit       ->setDateTime(m_gameFile->GetSaveTime());
    m_ui->playerXSpinBox     ->setValue(m_gameFile->GetPlayerPosition().X);
    m_ui->playerYSpinBox     ->setValue(m_gameFile->GetPlayerPosition().Y);
    m_ui->playerZSpinBox     ->setValue(m_gameFile->GetPlayerPosition().Z);
    m_ui->playerRollSpinBox  ->setValue(m_gameFile->GetPlayerRotation().X);
    m_ui->playerPitchSpinBox ->setValue(m_gameFile->GetPlayerRotation().Y);
    m_ui->playerYawSpinBox   ->setValue(m_gameFile->GetPlayerRotation().Z);
    m_ui->cameraXSpinBox     ->setValue(m_gameFile->GetCameraPosition().X);
    m_ui->cameraYSpinBox     ->setValue(m_gameFile->GetCameraPosition().Y);
    m_ui->cameraZSpinBox     ->setValue(m_gameFile->GetCameraPosition().Z);
    m_ui->cameraRollSpinBox  ->setValue(m_gameFile->GetCameraRotation().X);
    m_ui->cameraPitchSpinBox ->setValue(m_gameFile->GetCameraRotation().Y);
    m_ui->cameraYawSpinBox   ->setValue(m_gameFile->GetCameraRotation().Z);
    m_ui->roomIDSpinBox      ->setValue(m_gameFile->GetRoomID());
    m_ui->curMapLineEdit     ->setText(m_gameFile->GetCurrentMap());
    m_ui->curAreaLineEdit    ->setText(m_gameFile->GetCurrentArea());
    m_ui->curRoomLineEdit    ->setText(m_gameFile->GetCurrentRoom());
    m_ui->heroModeChkBox     ->setChecked(m_gameFile->IsHeroMode());
    m_ui->introViewedChkBox  ->setChecked(m_gameFile->GetIntroViewed());
    // Swords
    m_ui->practiceSwdChkBox  ->setChecked(m_gameFile->GetSword (SkywardSwordFile::PracticeSword));
    m_ui->goddessSwdChkBox   ->setChecked(m_gameFile->GetSword (SkywardSwordFile::GoddessSword));
    m_ui->longSwdChkBox      ->setChecked(m_gameFile->GetSword (SkywardSwordFile::LongSword));
    m_ui->whiteSwdChkBox     ->setChecked(m_gameFile->GetSword (SkywardSwordFile::WhiteSword));
    m_ui->masterSwdChkBox    ->setChecked(m_gameFile->GetSword (SkywardSwordFile::MasterSword));
    m_ui->trueMasterSwdChkBox->setChecked(m_gameFile->GetSword (SkywardSwordFile::TrueMasterSword));
    // Weapons
    m_ui->slingShotChkBox    ->setChecked(m_gameFile->GetEquipment(SkywardSwordFile::SlingshotWeapon));
    m_ui->scatterShotChkBox  ->setChecked(m_gameFile->GetEquipment(SkywardSwordFile::ScattershotWeapon));
    m_ui->bugNetChkBox       ->setChecked(m_gameFile->GetEquipment(SkywardSwordFile::BugnetWeapon));
    m_ui->bigBugNetChkBox    ->setChecked(m_gameFile->GetEquipment(SkywardSwordFile::BigBugnetWeapon));
    m_ui->beetleChkBox       ->setChecked(m_gameFile->GetEquipment(SkywardSwordFile::BeetleWeapon));
    m_ui->hookBeetleChkBox   ->setChecked(m_gameFile->GetEquipment(SkywardSwordFile::HookBeetleWeapon));
    m_ui->quickBeetleChkBox  ->setChecked(m_gameFile->GetEquipment(SkywardSwordFile::QuickBeetleWeapon));
    m_ui->toughBeetleChkBox  ->setChecked(m_gameFile->GetEquipment(SkywardSwordFile::ToughBeetleWeapon));
    m_ui->bombChkBox         ->setChecked(m_gameFile->GetEquipment(SkywardSwordFile::BombWeapon));
    m_ui->gustBellowsChkBox  ->setChecked(m_gameFile->GetEquipment(SkywardSwordFile::GustBellowsWeapon));
    m_ui->whipChkBox         ->setChecked(m_gameFile->GetEquipment(SkywardSwordFile::WhipWeapon));
    m_ui->clawShotChkBox     ->setChecked(m_gameFile->GetEquipment(SkywardSwordFile::ClawshotWeapon));
    m_ui->bowChkBox          ->setChecked(m_gameFile->GetEquipment(SkywardSwordFile::BowWeapon));
    m_ui->ironBowChkBox      ->setChecked(m_gameFile->GetEquipment(SkywardSwordFile::IronBowWeapon));
    m_ui->sacredBowChkBox    ->setChecked(m_gameFile->GetEquipment(SkywardSwordFile::SacredBowWeapon));
    m_ui->harpChkBox         ->setChecked(m_gameFile->GetEquipment(SkywardSwordFile::HarpEquipment));
    // Bugs
    m_ui->grassHopperChkBox  ->setChecked(m_gameFile->GetBug   (SkywardSwordFile::GrasshopperBug));
    m_ui->rhinoBeetleChkBox  ->setChecked(m_gameFile->GetBug   (SkywardSwordFile::RhinoBeetleBug));
    m_ui->mantisChkBox       ->setChecked(m_gameFile->GetBug   (SkywardSwordFile::MantisBug));
    m_ui->ladybugChkBox      ->setChecked(m_gameFile->GetBug   (SkywardSwordFile::LadybugBug));
    m_ui->butterflyChkBox    ->setChecked(m_gameFile->GetBug   (SkywardSwordFile::ButterflyBug));
    m_isUpdating = false;
}

void MainWindow::UpdateTitle()
{
    if (m_gameFile == NULL || !m_gameFile->IsOpen() || m_gameFile->GetGame() == SkywardSwordFile::GameNone)
        this->setWindowTitle(tr("WiiKing2 Editor"));
    else
    {
        QFileInfo fileInfo(m_gameFile->GetFilename());
        //HACK: Does this count as a hack?
        if (fileInfo.fileName().isEmpty())
            fileInfo.setFile(QDir(), "Untitled");

        this->setWindowTitle(QString(tr("WiiKing2 Editor (%1%2) - Game %3 0x"))
                             .arg(fileInfo.fileName())
                             .arg((m_gameFile->IsModified() ? "*" : ""))
                             .arg(m_gameFile->GetGame() + 1)
                             .append(QString("").sprintf("%08X", m_gameFile->GetChecksum())));
    }
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
