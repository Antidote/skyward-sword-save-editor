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
#include <QSettings>
#include <QFileSystemWatcher>
#include <QUrl>
#include "qhexedit2/qhexedit.h"
#include <QDebug>
#include <QtEndian>

#include "igamefile.h"
#include "skywardswordfile.h"
#include "newgamedialog.h"
#include "aboutdialog.h"
#include "fileinfodialog.h"
#include "preferencesdialog.h"
#include "newfiledialog.h"
#include "wiikeys.h"
#include "settingsmanager.h"

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
    m_isUpdating(false),
    m_isChecking(false),
    m_newFileDialog(NULL)
{
    m_fileWatcher = new QFileSystemWatcher;
    m_ui->setupUi(this);
    toggleWidgetStates();
    m_ui->tabWidget->setCurrentIndex(0);


    QSettings settings("WiiKing2", "WiiKing2 Editor");
    m_settingsManager = SettingsManager::instance();
    m_ui->actionPreferences->setEnabled(true);
    if (settings.allKeys().count() > 0)
    {
        qDebug() << "Registry entry found, attempting to load...";
        if(!WiiKeys::instance()->loadKeys())
        {
            qDebug() << "Couldn't not find 1 or more keys, attempting to load keys.bin...";
            if (!WiiKeys::instance()->open("./keys.bin"))
                qDebug() << "No keys.bin, requires manual entry";
            else
                qDebug() << "done";
        }
        else
            qDebug() << "done";
    }
    else
    {
        qDebug() << "No Registry Entry trying keys.bin";

        if (!WiiKeys::instance()->open("./keys.bin"))
            qDebug() << "No keys.bin, requires manual entry";
        else
            qDebug() << "done";
    }
    setupActions();
    setupHexEdit();
    setupConnections();
    // Now check to see if the user started the program with a commandline
    if (qApp->argc() > 1)
        // Attempt to open the file
    {
        m_gameFile = new SkywardSwordFile(qApp->arguments()[1]);
    }
    updateInfo();
    updateTitle();
    toggleWidgetStates();
}

MainWindow::~MainWindow()
{
    delete m_ui;
    delete m_settingsManager;
    if (m_gameFile != NULL)
    {
        if (m_gameFile->isOpen())
            m_gameFile->close();

        delete m_gameFile;
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    SkywardSwordFile::Region region;
    if (event->mimeData()->urls().count() == 1 && SkywardSwordFile::isValidFile(event->mimeData()->urls()[0].toLocalFile(), &region))
    {
        event->acceptProposedAction();
        statusBar()->showMessage(QString("File Valid (%1)").arg((region == SkywardSwordFile::NTSCURegion ? "NTSC-U" : region == SkywardSwordFile::NTSCJRegion ? "NTSC-J" : "PAL")));
        return;
    }

    if (event->mimeData()->urls().count() == 1)
    {
        QUrl url = event->mimeData()->urls()[0];
        if (url.toLocalFile().indexOf(".bin") == url.toLocalFile().length() - 4)
        {
            event->acceptProposedAction();
            statusBar()->showMessage(QString("File Valid (Wii Save file)"));

            return;
        }
    }

    statusBar()->showMessage(QString("Invalid File"));
}

void MainWindow::dragLeaveEvent(QDragLeaveEvent *event)
{
    statusBar()->clearMessage();

    QMainWindow::dragLeaveEvent(event);
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

        if (m_gameFile->open(m_curGame, mimeData->urls()[0].toLocalFile()))
        {
            m_hexEdit->setData(m_gameFile->gameData());
            updateInfo();
            updateTitle();
        }
    }
    statusBar()->clearMessage();
}

void MainWindow::setupActions()
{
    // File -> Open
    m_ui->actionOpen->setShortcuts(QKeySequence::Open);
    m_ui->actionOpen->setStatusTip(tr("Opens a Skyward Sword save file..."));
    // File -> New
    m_ui->actionNew->setShortcuts(QKeySequence::New);
    m_ui->actionNew->setStatusTip(tr("Creates a new Skyward Sword save file"));
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
void MainWindow::setupHexEdit()
{
    // Setup the hex edit widget
    m_hexEdit   = new QHexEdit;
    m_hexEdit->setOverwriteMode(true);
    m_hexEdit->setInsertAllowed(false);
    m_ui->hexEditLayout->addWidget(m_hexEdit);

    // Setup the inspector
    QStandardItemModel* model = new QStandardItemModel(0, 2, this);
    model->setHorizontalHeaderItem(0, new QStandardItem("Type"));
    model->setHorizontalHeaderItem(1, new QStandardItem("Value"));
    model->appendRow(new QStandardItem("char"));
    model->appendRow(new QStandardItem("uchar"));
    model->appendRow(new QStandardItem("int32"));
    model->appendRow(new QStandardItem("uint32"));
    model->appendRow(new QStandardItem("int64"));
    model->appendRow(new QStandardItem("uint64"));
    m_ui->inspectorView->setModel(model);
}

void MainWindow::setupConnections()
{
    connect(m_fileWatcher,              SIGNAL(fileChanged(QString)), this, SLOT(onFileChanged(QString)));
    connect(m_hexEdit,                  SIGNAL(currentAddressChanged(int)), this, SLOT(onCurrentAdressChanged(int)));
    connect(m_hexEdit,                  SIGNAL(dataChanged()),        this, SLOT(onHexDataChanged()));
    connect(m_ui->hexGoToBtn,           SIGNAL(clicked()),            this, SLOT(onHexGotoAddress()));
    connect(m_ui->hexUndoBtn,           SIGNAL(clicked()),            m_hexEdit, SLOT(undo()));
    connect(m_ui->hexRedoBtn,           SIGNAL(clicked()),            m_hexEdit, SLOT(redo()));
    connect(m_ui->playHoursSpinBox,     SIGNAL(valueChanged(int)),    this, SLOT(onValueChanged()));
    connect(m_ui->playMinutesSpinBox,   SIGNAL(valueChanged(int)),    this, SLOT(onValueChanged()));
    connect(m_ui->playSecondsSpinBox,   SIGNAL(valueChanged(int)),    this, SLOT(onValueChanged()));
    connect(m_ui->saveTimeEdit,         SIGNAL(dateTimeChanged(QDateTime)), this, SLOT(onValueChanged()));
    connect(m_ui->playerXSpinBox,       SIGNAL(valueChanged(double)), this, SLOT(onValueChanged()));
    connect(m_ui->playerYSpinBox,       SIGNAL(valueChanged(double)), this, SLOT(onValueChanged()));
    connect(m_ui->playerZSpinBox,       SIGNAL(valueChanged(double)), this, SLOT(onValueChanged()));
    connect(m_ui->playerRollSpinBox,    SIGNAL(valueChanged(double)), this, SLOT(onValueChanged()));
    connect(m_ui->playerPitchSpinBox,   SIGNAL(valueChanged(double)), this, SLOT(onValueChanged()));
    connect(m_ui->playerYawSpinBox,     SIGNAL(valueChanged(double)), this, SLOT(onValueChanged()));
    connect(m_ui->cameraXSpinBox,       SIGNAL(valueChanged(double)), this, SLOT(onValueChanged()));
    connect(m_ui->cameraYSpinBox,       SIGNAL(valueChanged(double)), this, SLOT(onValueChanged()));
    connect(m_ui->cameraZSpinBox,       SIGNAL(valueChanged(double)), this, SLOT(onValueChanged()));
    connect(m_ui->cameraRollSpinBox,    SIGNAL(valueChanged(double)), this, SLOT(onValueChanged()));
    connect(m_ui->cameraPitchSpinBox,   SIGNAL(valueChanged(double)), this, SLOT(onValueChanged()));
    connect(m_ui->cameraYawSpinBox,     SIGNAL(valueChanged(double)), this, SLOT(onValueChanged()));
    connect(m_ui->nightChkbox,          SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->heroModeChkBox,       SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->introViewedChkBox,    SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->nameLineEdit,         SIGNAL(textChanged(QString)), this, SLOT(onTextChanged(QString)));
    connect(m_ui->curMapLineEdit,       SIGNAL(textChanged(QString)), this, SLOT(onTextChanged(QString)));
    connect(m_ui->curAreaLineEdit,      SIGNAL(textChanged(QString)), this, SLOT(onTextChanged(QString)));
    connect(m_ui->curRoomLineEdit,      SIGNAL(textChanged(QString)), this, SLOT(onTextChanged(QString)));
    connect(m_gameGroup,                SIGNAL(triggered(QAction*)),  this, SLOT(onGameChanged(QAction*)));
    connect(m_ui->actionOpen,           SIGNAL(triggered()),          this, SLOT(onOpen()));
    connect(m_ui->actionNew,            SIGNAL(triggered()),          this, SLOT(onNew()));
    connect(m_ui->createDeleteGameBtn,  SIGNAL(clicked()),            this, SLOT(onCreateNewGame()));
    connect(m_ui->actionSave,           SIGNAL(triggered()),          this, SLOT(onSave()));
    connect(m_ui->actionSaveAs,         SIGNAL(triggered()),          this, SLOT(onSaveAs()));
    connect(m_ui->actionClose,          SIGNAL(triggered()),          this, SLOT(onClose()));
    connect(m_ui->actionReload,         SIGNAL(triggered()),          this, SLOT(onReload()));
    connect(m_ui->actionExit,           SIGNAL(triggered()),          this, SLOT(close()));
    connect(m_ui->actionAbout,          SIGNAL(triggered()),          this, SLOT(onAbout()));
    connect(m_ui->actionAboutQt,        SIGNAL(triggered()),          this, SLOT(onAboutQt()));
    connect(m_ui->actionFileInfo,       SIGNAL(triggered()),          this, SLOT(onFileInfo()));
    connect(m_ui->actionPreferences,    SIGNAL(triggered()),          this, SLOT(onPreferences()));
    // Swords
    connect(m_ui->practiceSwdChkBox,    SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->goddessSwdChkBox,     SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->longSwdChkBox,        SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->whiteSwdChkBox,       SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->masterSwdChkBox,      SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->trueMasterSwdChkBox,  SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    // Weapons
    connect(m_ui->slingShotChkBox,      SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->scatterShotChkBox,    SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->bugNetChkBox,         SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->bigBugNetChkBox,      SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->beetleChkBox,         SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->hookBeetleChkBox,     SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->quickBeetleChkBox,    SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->toughBeetleChkBox,    SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->bombChkBox,           SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->gustBellowsChkBox,    SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->whipChkBox,           SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->clawShotChkBox,       SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->bowChkBox,            SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->ironBowChkBox,        SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->sacredBowChkBox,      SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->diggingMittsChkBox,   SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->moleMittsChkBox,      SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->sailClothChkBox,      SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->harpChkBox,           SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->dragonScaleChkBox,    SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->fireEaringsChkBox,    SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    // Bugs
    connect(m_ui->hornetChkBox,         SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->butterflyChkBox,      SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->dragonflyChkBox,      SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->fireflyChkBox,        SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->rhinoBeetleChkBox,    SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->ladybugChkBox,        SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->sandCicadaChkBox,     SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->stagBeetleChkBox,     SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->grasshopperChkBox,    SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->mantisChkBox,         SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->antChkBox,            SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->eldinRollerChkBox,    SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    // Materials
    connect(m_ui->hornetLarvaeChkBox,   SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->birdFeatherChkBox,    SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->tumbleWeedChkBox,     SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->lizardTailChkBox,     SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->eldinOreChkBox,       SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->ancientFlowerChkBox,  SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->amberRelicChkBox,     SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->duskRelicChkBox,      SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->jellyBlobChkBox,      SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->monsterClawChkBox,    SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->monsterHornChkBox,    SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->decoSkullChkBox,      SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->evilCrystalChkBox,    SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->blueBirdFeatherChkBox,SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->goldenSkullChkBox,    SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->goddessPlumeChkBox,   SIGNAL(toggled(bool)),        this, SLOT(onValueChanged()));
    connect(m_ui->rupeeSpinBox,         SIGNAL(valueChanged(int)),    this, SLOT(onValueChanged()));
    connect(m_ui->totalHPSpinBox,       SIGNAL(valueChanged(int)),    this, SLOT(onValueChanged()));
    connect(m_ui->unkHPSpinBox,         SIGNAL(valueChanged(int)),    this, SLOT(onValueChanged()));
    connect(m_ui->curHPSpinBox,         SIGNAL(valueChanged(int)),    this, SLOT(onValueChanged()));
    connect(m_ui->roomIDSpinBox,        SIGNAL(valueChanged(int)),    this, SLOT(onValueChanged()));

    // Amounts
    // Ammo
    connect(m_ui->arrowAmmoSpinBox,   SIGNAL(valueChanged(int)), this, SLOT(onValueChanged()));
    connect(m_ui->bombAmmoSpinBox,    SIGNAL(valueChanged(int)), this, SLOT(onValueChanged()));
    connect(m_ui->seedAmmoSpinBox,    SIGNAL(valueChanged(int)), this, SLOT(onValueChanged()));
    // Bugs
    connect(m_ui->hornetSpinBox,      SIGNAL(valueChanged(int)), this, SLOT(onValueChanged()));
    connect(m_ui->butterflySpinBox,   SIGNAL(valueChanged(int)), this, SLOT(onValueChanged()));
    connect(m_ui->dragonflySpinBox,   SIGNAL(valueChanged(int)), this, SLOT(onValueChanged()));
    connect(m_ui->fireflySpinBox,     SIGNAL(valueChanged(int)), this, SLOT(onValueChanged()));
    connect(m_ui->rhinoBeetleSpinBox, SIGNAL(valueChanged(int)), this, SLOT(onValueChanged()));
    connect(m_ui->ladybugSpinBox,     SIGNAL(valueChanged(int)), this, SLOT(onValueChanged()));
    connect(m_ui->sandCicadaSpinBox,  SIGNAL(valueChanged(int)), this, SLOT(onValueChanged()));
    connect(m_ui->stagBeetleSpinBox,  SIGNAL(valueChanged(int)), this, SLOT(onValueChanged()));
    connect(m_ui->grasshopperSpinBox, SIGNAL(valueChanged(int)), this, SLOT(onValueChanged()));
    connect(m_ui->mantisSpinBox,      SIGNAL(valueChanged(int)), this, SLOT(onValueChanged()));
    connect(m_ui->antSpinBox,         SIGNAL(valueChanged(int)), this, SLOT(onValueChanged()));
    connect(m_ui->eldinRollerSpinBox, SIGNAL(valueChanged(int)), this, SLOT(onValueChanged()));
    // Materials
    connect(m_ui->hornetLarvaeSpinBox,   SIGNAL(valueChanged(int)), this, SLOT(onValueChanged()));
    connect(m_ui->birdFeatherSpinBox,    SIGNAL(valueChanged(int)), this, SLOT(onValueChanged()));
    connect(m_ui->tumbleWeedSpinBox,     SIGNAL(valueChanged(int)), this, SLOT(onValueChanged()));
    connect(m_ui->lizardTailSpinBox,     SIGNAL(valueChanged(int)), this, SLOT(onValueChanged()));
    connect(m_ui->eldinOreSpinBox,       SIGNAL(valueChanged(int)), this, SLOT(onValueChanged()));
    connect(m_ui->ancientFlowerSpinBox,  SIGNAL(valueChanged(int)), this, SLOT(onValueChanged()));
    connect(m_ui->amberRelicSpinBox,     SIGNAL(valueChanged(int)), this, SLOT(onValueChanged()));
    connect(m_ui->duskRelicSpinBox,      SIGNAL(valueChanged(int)), this, SLOT(onValueChanged()));
    connect(m_ui->jellyBlobSpinBox,      SIGNAL(valueChanged(int)), this, SLOT(onValueChanged()));
    connect(m_ui->monsterClawSpinBox,    SIGNAL(valueChanged(int)), this, SLOT(onValueChanged()));
    connect(m_ui->monsterHornSpinBox,    SIGNAL(valueChanged(int)), this, SLOT(onValueChanged()));
    connect(m_ui->decoSkullSpinBox,      SIGNAL(valueChanged(int)), this, SLOT(onValueChanged()));
    connect(m_ui->evilCrystalSpinBox,    SIGNAL(valueChanged(int)), this, SLOT(onValueChanged()));
    connect(m_ui->blueBirdFeatherSpinBox,SIGNAL(valueChanged(int)), this, SLOT(onValueChanged()));
    connect(m_ui->goldenSkullSpinBox,    SIGNAL(valueChanged(int)), this, SLOT(onValueChanged()));
    connect(m_ui->goddessPlumeSpinBox,   SIGNAL(valueChanged(int)), this, SLOT(onValueChanged()));
    // Gratitude Crystal
    connect(m_ui->gratitudeCrystalSpinBox, SIGNAL(valueChanged(int)), this, SLOT(onValueChanged()));
}

SkywardSwordFile* MainWindow::gameFile()
{
    return m_gameFile;
}

void MainWindow::onTextChanged(QString text)
{
    if (!m_gameFile || !m_gameFile->isOpen() ||
        m_isUpdating || m_gameFile->game() == SkywardSwordFile::GameNone)
        return;

    if (m_ui->nameLineEdit->isModified())
    {
        m_ui->nameLineEdit->setModified(false);
        m_gameFile->setPlayerName(text);
    }

    if (m_ui->curMapLineEdit->isModified())
    {
        m_ui->curMapLineEdit->setModified(false);
        m_gameFile->setCurrentMap(text);
    }

    if (m_ui->curAreaLineEdit->isModified())
    {
        m_ui->curAreaLineEdit->setModified(false);
        m_gameFile->setCurrentArea(text);
    }

    if (m_ui->curRoomLineEdit->isModified())
    {
        m_ui->curRoomLineEdit->setModified(false);
        m_gameFile->setCurrentRoom(text);
    }

    m_gameFile->updateChecksum();
    m_hexEdit->setData(m_gameFile->gameData());
    updateTitle();
}

void MainWindow::onCurrentAdressChanged(int address)
{
   qint8   charVal;
   quint8  ucharVal;
   qint32  int32Val;
   quint32 uint32Val;
   qint32  int64Val;
   quint64 uint64Val;

   if ((address + 1) <= m_hexEdit->data().size())
   {
       charVal = *(qint8*)(m_hexEdit->data().data() + address);
       ucharVal = *(quint8*)(m_hexEdit->data().data() + address);
   }
   else
   {
       charVal = 0;
       ucharVal = 0;
   }

   if ((address + 4) <= m_hexEdit->data().size())
   {
       int32Val = qFromBigEndian(*(qint32*)(m_hexEdit->data().data() + address));
       uint32Val = qFromBigEndian(*(qint32*)(m_hexEdit->data().data() + address));
   }
   else
   {
       int32Val = 0;
       uint32Val = 0;
   }

   if ((address + 8) <= m_hexEdit->data().size())
   {
       int64Val = qFromBigEndian(*(qint64*)(m_hexEdit->data().data() + address));
       uint64Val = qFromBigEndian(*(qint64*)(m_hexEdit->data().data() + address));
   }
   else
   {
       int64Val = 0;
       uint64Val = 0;
   }

   ((QStandardItemModel*)m_ui->inspectorView->model())->setItem(0, 1, new QStandardItem(QString("%1").arg((qint32)charVal)));
   ((QStandardItemModel*)m_ui->inspectorView->model())->setItem(1, 1, new QStandardItem(QString("%1 '%2'").arg((quint32)ucharVal).arg(charVal)));
   ((QStandardItemModel*)m_ui->inspectorView->model())->setItem(2, 1, new QStandardItem(QString("%1").arg(int32Val)));
   ((QStandardItemModel*)m_ui->inspectorView->model())->setItem(3, 1, new QStandardItem(QString("%1").arg(uint32Val)));
   ((QStandardItemModel*)m_ui->inspectorView->model())->setItem(4, 1, new QStandardItem(QString("%1").arg(int64Val)));
   ((QStandardItemModel*)m_ui->inspectorView->model())->setItem(5, 1, new QStandardItem(QString("%1").arg(uint64Val)));

   m_ui->hexOffsetLbl->setText(QString("Offset " + QString("%1").arg(address, 4, 16, QLatin1Char('0')).toUpper()));
}

void MainWindow::onHexDataChanged()
{
    if (!m_gameFile)
        return;

    m_ui->hexRedoBtn->setEnabled(m_hexEdit->undoStack()->canRedo());
    m_ui->hexUndoBtn->setEnabled(m_hexEdit->undoStack()->canUndo());
    m_gameFile->setGameData(m_hexEdit->data());
    m_gameFile->updateChecksum();
    updateInfo();
    updateTitle();
}

void MainWindow::onHexGotoAddress()
{
    if (!m_ui->hexGoToLineEdit->text().isEmpty())
    {
        bool ok;
        int ret = m_ui->hexGoToLineEdit->text().toInt(&ok);
        if (!ok)
        {
            ret = m_ui->hexGoToLineEdit->text().toInt(&ok, 16);
            if (!ok)
            {
                statusBar()->showMessage(tr("Invalid Address \"%1\"").arg(m_ui->hexGoToLineEdit->text()));
                return;
            }
        }

        //m_hexEdit->setAddressOffset(ret);
        m_hexEdit->setCursorPosition(ret);
    }
}

void MainWindow::updateInfo()
{
    if (!m_gameFile || !m_gameFile->isOpen() ||
        m_isUpdating || m_gameFile->game() == SkywardSwordFile::GameNone)
        return;

    if (!m_gameFile->isNew())
    {
        m_ui->createDeleteGameBtn->setText(tr("Delete Adventure"));
        if (m_ui->createDeleteGameBtn->disconnect())
            connect(m_ui->createDeleteGameBtn, SIGNAL(clicked()), this, SLOT(onDeleteGame()));

        m_ui->tabWidget->setEnabled(true);
    }
    else
    {
        m_ui->createDeleteGameBtn->setText(tr("Click to create a new Adventure"));
        if (m_ui->createDeleteGameBtn->disconnect())
            connect(m_ui->createDeleteGameBtn, SIGNAL(clicked()), this, SLOT(onCreateNewGame()));

        m_ui->tabWidget->setEnabled(false);
    }
    toggleWidgetStates();

    m_isUpdating = true;
    // Player Stats
    m_ui->nameLineEdit       ->setText(m_gameFile->playerName());
    m_ui->rupeeSpinBox       ->setValue(m_gameFile->rupees());
    m_ui->totalHPSpinBox     ->setValue(m_gameFile->totalHP());
    m_ui->unkHPSpinBox       ->setValue(m_gameFile->unkHP());
    m_ui->curHPSpinBox       ->setValue(m_gameFile->currentHP());
    m_ui->playHoursSpinBox   ->setValue(m_gameFile->playTime().Hours);
    m_ui->playMinutesSpinBox ->setValue(m_gameFile->playTime().Minutes);
    m_ui->playSecondsSpinBox ->setValue(m_gameFile->playTime().Seconds);
    m_ui->saveTimeEdit       ->setDateTime(m_gameFile->saveTime());
    m_ui->playerXSpinBox     ->setValue(m_gameFile->playerPosition().X);
    m_ui->playerYSpinBox     ->setValue(m_gameFile->playerPosition().Y);
    m_ui->playerZSpinBox     ->setValue(m_gameFile->playerPosition().Z);
    m_ui->playerRollSpinBox  ->setValue(m_gameFile->playerRotation().X);
    m_ui->playerPitchSpinBox ->setValue(m_gameFile->playerRotation().Y);
    m_ui->playerYawSpinBox   ->setValue(m_gameFile->playerRotation().Z);
    m_ui->cameraXSpinBox     ->setValue(m_gameFile->cameraPosition().X);
    m_ui->cameraYSpinBox     ->setValue(m_gameFile->cameraPosition().Y);
    m_ui->cameraZSpinBox     ->setValue(m_gameFile->cameraPosition().Z);
    m_ui->cameraRollSpinBox  ->setValue(m_gameFile->cameraRotation().X);
    m_ui->cameraPitchSpinBox ->setValue(m_gameFile->cameraRotation().Y);
    m_ui->cameraYawSpinBox   ->setValue(m_gameFile->cameraRotation().Z);
    m_ui->roomIDSpinBox      ->setValue(m_gameFile->roomID());
    m_ui->curMapLineEdit     ->setText(m_gameFile->currentMap());
    m_ui->curAreaLineEdit    ->setText(m_gameFile->currentArea());
    m_ui->curRoomLineEdit    ->setText(m_gameFile->currentRoom());
    m_ui->nightChkbox        ->setChecked(m_gameFile->isNight());
    m_ui->heroModeChkBox     ->setChecked(m_gameFile->isHeroMode());
    m_ui->introViewedChkBox  ->setChecked(m_gameFile->introViewed());
    // Swords
    m_ui->practiceSwdChkBox  ->setChecked(m_gameFile->sword (SkywardSwordFile::PracticeSword));
    m_ui->goddessSwdChkBox   ->setChecked(m_gameFile->sword (SkywardSwordFile::GoddessSword));
    m_ui->longSwdChkBox      ->setChecked(m_gameFile->sword (SkywardSwordFile::LongSword));
    m_ui->whiteSwdChkBox     ->setChecked(m_gameFile->sword (SkywardSwordFile::WhiteSword));
    m_ui->masterSwdChkBox    ->setChecked(m_gameFile->sword (SkywardSwordFile::MasterSword));
    m_ui->trueMasterSwdChkBox->setChecked(m_gameFile->sword (SkywardSwordFile::TrueMasterSword));
    // Weapons
    m_ui->slingShotChkBox    ->setChecked(m_gameFile->equipment(SkywardSwordFile::SlingshotWeapon));
    m_ui->scatterShotChkBox  ->setChecked(m_gameFile->equipment(SkywardSwordFile::ScattershotWeapon));
    m_ui->bugNetChkBox       ->setChecked(m_gameFile->equipment(SkywardSwordFile::BugnetWeapon));
    m_ui->bigBugNetChkBox    ->setChecked(m_gameFile->equipment(SkywardSwordFile::BigBugnetWeapon));
    m_ui->beetleChkBox       ->setChecked(m_gameFile->equipment(SkywardSwordFile::BeetleWeapon));
    m_ui->hookBeetleChkBox   ->setChecked(m_gameFile->equipment(SkywardSwordFile::HookBeetleWeapon));
    m_ui->quickBeetleChkBox  ->setChecked(m_gameFile->equipment(SkywardSwordFile::QuickBeetleWeapon));
    m_ui->toughBeetleChkBox  ->setChecked(m_gameFile->equipment(SkywardSwordFile::ToughBeetleWeapon));
    m_ui->bombChkBox         ->setChecked(m_gameFile->equipment(SkywardSwordFile::BombWeapon));
    m_ui->gustBellowsChkBox  ->setChecked(m_gameFile->equipment(SkywardSwordFile::GustBellowsWeapon));
    m_ui->whipChkBox         ->setChecked(m_gameFile->equipment(SkywardSwordFile::WhipWeapon));
    m_ui->clawShotChkBox     ->setChecked(m_gameFile->equipment(SkywardSwordFile::ClawshotWeapon));
    m_ui->bowChkBox          ->setChecked(m_gameFile->equipment(SkywardSwordFile::BowWeapon));
    m_ui->ironBowChkBox      ->setChecked(m_gameFile->equipment(SkywardSwordFile::IronBowWeapon));
    m_ui->sacredBowChkBox    ->setChecked(m_gameFile->equipment(SkywardSwordFile::SacredBowWeapon));
    m_ui->diggingMittsChkBox ->setChecked(m_gameFile->equipment(SkywardSwordFile::DiggingMittsEquipment));
    m_ui->moleMittsChkBox    ->setChecked(m_gameFile->equipment(SkywardSwordFile::MoleMittsEquipment));
    m_ui->sailClothChkBox    ->setChecked(m_gameFile->equipment(SkywardSwordFile::SailClothEquipment));
    m_ui->harpChkBox         ->setChecked(m_gameFile->equipment(SkywardSwordFile::HarpEquipment));
    m_ui->dragonScaleChkBox  ->setChecked(m_gameFile->equipment(SkywardSwordFile::WaterDragonScaleEquipment));
    m_ui->fireEaringsChkBox  ->setChecked(m_gameFile->equipment(SkywardSwordFile::FireShieldEaringsEquipment));
    // Bugs
    m_ui->hornetChkBox       ->setChecked(m_gameFile->bug(SkywardSwordFile::HornetBug));
    m_ui->butterflyChkBox    ->setChecked(m_gameFile->bug(SkywardSwordFile::ButterflyBug));
    m_ui->dragonflyChkBox    ->setChecked(m_gameFile->bug(SkywardSwordFile::DragonflyBug));
    m_ui->fireflyChkBox      ->setChecked(m_gameFile->bug(SkywardSwordFile::FireflyBug));
    m_ui->rhinoBeetleChkBox  ->setChecked(m_gameFile->bug(SkywardSwordFile::RhinoBeetleBug));
    m_ui->ladybugChkBox      ->setChecked(m_gameFile->bug(SkywardSwordFile::LadybugBug));
    m_ui->sandCicadaChkBox   ->setChecked(m_gameFile->bug(SkywardSwordFile::SandCicadaBug));
    m_ui->stagBeetleChkBox   ->setChecked(m_gameFile->bug(SkywardSwordFile::StagBeetleBug));
    m_ui->grasshopperChkBox  ->setChecked(m_gameFile->bug(SkywardSwordFile::GrasshopperBug));
    m_ui->mantisChkBox       ->setChecked(m_gameFile->bug(SkywardSwordFile::MantisBug));
    m_ui->antChkBox          ->setChecked(m_gameFile->bug(SkywardSwordFile::AntBug));
    m_ui->eldinRollerChkBox  ->setChecked(m_gameFile->bug(SkywardSwordFile::RollerBug));
    // Materials
    m_ui->hornetLarvaeChkBox  ->setChecked(m_gameFile->material(SkywardSwordFile::HornetLarvaeMaterial));
    m_ui->birdFeatherChkBox   ->setChecked(m_gameFile->material(SkywardSwordFile::BirdFeatherMaterial));
    m_ui->tumbleWeedChkBox    ->setChecked(m_gameFile->material(SkywardSwordFile::TumbleWeedMaterial));
    m_ui->lizardTailChkBox    ->setChecked(m_gameFile->material(SkywardSwordFile::LizardTailMaterial));
    m_ui->eldinOreChkBox      ->setChecked(m_gameFile->material(SkywardSwordFile::EldinOreMaterial));
    m_ui->ancientFlowerChkBox ->setChecked(m_gameFile->material(SkywardSwordFile::AncientFlowerMaterial));
    m_ui->amberRelicChkBox    ->setChecked(m_gameFile->material(SkywardSwordFile::AmberRelicMaterial));
    m_ui->duskRelicChkBox     ->setChecked(m_gameFile->material(SkywardSwordFile::DuskRelicMaterial));
    m_ui->jellyBlobChkBox     ->setChecked(m_gameFile->material(SkywardSwordFile::JellyBlobMaterial));
    m_ui->monsterClawChkBox   ->setChecked(m_gameFile->material(SkywardSwordFile::MonsterClawMaterial));
    m_ui->monsterHornChkBox   ->setChecked(m_gameFile->material(SkywardSwordFile::MonsterHornMaterial));
    m_ui->decoSkullChkBox     ->setChecked(m_gameFile->material(SkywardSwordFile::OrnamentalSkullMaterial));
    m_ui->evilCrystalChkBox   ->setChecked(m_gameFile->material(SkywardSwordFile::EvilCrystalMaterial));
    m_ui->blueBirdFeatherChkBox->setChecked(m_gameFile->material(SkywardSwordFile::BlueBirdFeatherMaterial));
    m_ui->goldenSkullChkBox   ->setChecked(m_gameFile->material(SkywardSwordFile::GoldenSkullMaterial));
    m_ui->goddessPlumeChkBox  ->setChecked(m_gameFile->material(SkywardSwordFile::GoddessPlumeMaterial));

    // Quantities

    // Ammo
    m_ui->arrowAmmoSpinBox->setValue(m_gameFile->ammo(SkywardSwordFile::ArrowAmmo));
    m_ui->arrowAmmoSpinBox->setEnabled(m_ui->bowChkBox->isChecked() || m_ui->ironBowChkBox->isChecked() || m_ui->sacredBowChkBox->isChecked());
    m_ui->bombAmmoSpinBox->setValue(m_gameFile->ammo(SkywardSwordFile::BombAmmo));
    m_ui->bombAmmoSpinBox->setEnabled(m_ui->bombChkBox->isChecked());
    m_ui->seedAmmoSpinBox->setValue(m_gameFile->ammo(SkywardSwordFile::SeedAmmo));
    m_ui->seedAmmoSpinBox->setEnabled(m_ui->slingShotChkBox->isChecked() || m_ui->scatterShotChkBox->isChecked());
    // Bugs
    m_ui->hornetSpinBox     ->setValue(m_gameFile->bugQuantity(SkywardSwordFile::HornetBug));
    m_ui->hornetSpinBox     ->setEnabled(m_ui->hornetChkBox->isChecked());
    m_ui->butterflySpinBox  ->setValue(m_gameFile->bugQuantity(SkywardSwordFile::ButterflyBug));
    m_ui->butterflySpinBox  ->setEnabled(m_ui->butterflyChkBox->isChecked());
    m_ui->dragonflySpinBox  ->setValue(m_gameFile->bugQuantity(SkywardSwordFile::DragonflyBug));
    m_ui->dragonflySpinBox  ->setEnabled(m_ui->dragonflyChkBox->isChecked());
    m_ui->fireflySpinBox    ->setValue(m_gameFile->bugQuantity(SkywardSwordFile::FireflyBug));
    m_ui->fireflySpinBox    ->setEnabled(m_ui->fireflyChkBox->isChecked());
    m_ui->rhinoBeetleSpinBox->setValue(m_gameFile->bugQuantity(SkywardSwordFile::RhinoBeetleBug));
    m_ui->rhinoBeetleSpinBox->setEnabled(m_ui->rhinoBeetleChkBox->isChecked());
    m_ui->ladybugSpinBox    ->setValue(m_gameFile->bugQuantity(SkywardSwordFile::LadybugBug));
    m_ui->ladybugSpinBox    ->setEnabled(m_ui->ladybugChkBox->isChecked());
    m_ui->sandCicadaSpinBox ->setValue(m_gameFile->bugQuantity(SkywardSwordFile::SandCicadaBug));
    m_ui->sandCicadaSpinBox ->setEnabled(m_ui->sandCicadaChkBox->isChecked());
    m_ui->stagBeetleSpinBox ->setValue(m_gameFile->bugQuantity(SkywardSwordFile::StagBeetleBug));
    m_ui->stagBeetleSpinBox ->setEnabled(m_ui->stagBeetleChkBox->isChecked());
    m_ui->grasshopperSpinBox->setValue(m_gameFile->bugQuantity(SkywardSwordFile::GrasshopperBug));
    m_ui->grasshopperSpinBox->setEnabled(m_ui->grasshopperChkBox->isChecked());
    m_ui->mantisSpinBox     ->setValue(m_gameFile->bugQuantity(SkywardSwordFile::MantisBug));
    m_ui->mantisSpinBox     ->setEnabled(m_ui->mantisChkBox->isChecked());
    m_ui->antSpinBox        ->setValue(m_gameFile->bugQuantity(SkywardSwordFile::AntBug));
    m_ui->antSpinBox        ->setEnabled(m_ui->antChkBox->isChecked());
    m_ui->eldinRollerSpinBox->setValue(m_gameFile->bugQuantity(SkywardSwordFile::RollerBug));
    m_ui->eldinRollerSpinBox->setEnabled(m_ui->eldinRollerChkBox->isChecked());

    // Materials
    m_ui->hornetLarvaeSpinBox    ->setValue(m_gameFile->materialQuantity(SkywardSwordFile::HornetLarvaeMaterial));
    m_ui->hornetLarvaeSpinBox    ->setEnabled(m_ui->hornetLarvaeChkBox->isChecked());
    m_ui->birdFeatherSpinBox     ->setValue(m_gameFile->materialQuantity(SkywardSwordFile::BirdFeatherMaterial));
    m_ui->birdFeatherSpinBox     ->setEnabled(m_ui->birdFeatherChkBox->isChecked());
    m_ui->tumbleWeedSpinBox      ->setValue(m_gameFile->materialQuantity(SkywardSwordFile::TumbleWeedMaterial));
    m_ui->tumbleWeedSpinBox      ->setEnabled(m_ui->tumbleWeedChkBox->isChecked());
    m_ui->lizardTailSpinBox      ->setValue(m_gameFile->materialQuantity(SkywardSwordFile::LizardTailMaterial));
    m_ui->lizardTailSpinBox      ->setEnabled(m_ui->lizardTailChkBox->isChecked());
    m_ui->eldinOreSpinBox        ->setValue(m_gameFile->materialQuantity(SkywardSwordFile::EldinOreMaterial));
    m_ui->eldinOreSpinBox        ->setEnabled(m_ui->eldinOreChkBox->isChecked());
    m_ui->ancientFlowerSpinBox   ->setValue(m_gameFile->materialQuantity(SkywardSwordFile::AncientFlowerMaterial));
    m_ui->ancientFlowerSpinBox   ->setEnabled(m_ui->ancientFlowerChkBox->isChecked());
    m_ui->amberRelicSpinBox      ->setValue(m_gameFile->materialQuantity(SkywardSwordFile::AmberRelicMaterial));
    m_ui->amberRelicSpinBox      ->setEnabled(m_ui->amberRelicChkBox->isChecked());
    m_ui->duskRelicSpinBox       ->setValue(m_gameFile->materialQuantity(SkywardSwordFile::DuskRelicMaterial));
    m_ui->duskRelicSpinBox       ->setEnabled(m_ui->duskRelicChkBox->isChecked());
    m_ui->jellyBlobSpinBox       ->setValue(m_gameFile->materialQuantity(SkywardSwordFile::JellyBlobMaterial));
    m_ui->jellyBlobSpinBox       ->setEnabled(m_ui->jellyBlobChkBox->isChecked());
    m_ui->monsterClawSpinBox     ->setValue(m_gameFile->materialQuantity(SkywardSwordFile::MonsterClawMaterial));
    m_ui->monsterClawSpinBox     ->setEnabled(m_ui->monsterClawChkBox->isChecked());
    m_ui->monsterHornSpinBox     ->setValue(m_gameFile->materialQuantity(SkywardSwordFile::MonsterHornMaterial));
    m_ui->monsterHornSpinBox     ->setEnabled(m_ui->monsterHornChkBox->isChecked());
    m_ui->decoSkullSpinBox       ->setValue(m_gameFile->materialQuantity(SkywardSwordFile::OrnamentalSkullMaterial));
    m_ui->decoSkullSpinBox       ->setEnabled(m_ui->decoSkullChkBox->isChecked());
    m_ui->evilCrystalSpinBox     ->setValue(m_gameFile->materialQuantity(SkywardSwordFile::EvilCrystalMaterial));
    m_ui->evilCrystalSpinBox     ->setEnabled(m_ui->evilCrystalChkBox->isChecked());
    m_ui->blueBirdFeatherSpinBox ->setValue(m_gameFile->materialQuantity(SkywardSwordFile::BlueBirdFeatherMaterial));
    m_ui->blueBirdFeatherSpinBox ->setEnabled(m_ui->blueBirdFeatherChkBox->isChecked());
    m_ui->goldenSkullSpinBox     ->setValue(m_gameFile->materialQuantity(SkywardSwordFile::GoldenSkullMaterial));
    m_ui->goldenSkullSpinBox     ->setEnabled(m_ui->goldenSkullChkBox->isChecked());
    m_ui->goddessPlumeSpinBox    ->setValue(m_gameFile->materialQuantity(SkywardSwordFile::GoddessPlumeMaterial));
    m_ui->goddessPlumeSpinBox    ->setEnabled(m_ui->goddessPlumeChkBox->isChecked());

    m_ui->gratitudeCrystalSpinBox->setValue(m_gameFile->gratitudeCrystalAmount());

    m_isUpdating = false;
}

void MainWindow::onValueChanged()
{
    if (!m_gameFile || !m_gameFile->isOpen() ||
        m_isUpdating || m_gameFile->game() == SkywardSwordFile::GameNone)
        return;
    PlayTime playTime;
    playTime.Hours = m_ui->playHoursSpinBox->value();
    playTime.Minutes = m_ui->playMinutesSpinBox->value();
    playTime.Seconds = m_ui->playSecondsSpinBox->value();
    m_gameFile->setPlayTime(playTime);
    m_gameFile->setPlayerPosition((float)m_ui->playerXSpinBox->value(),    (float)m_ui->playerYSpinBox->value(),     (float)m_ui->playerZSpinBox->value());
    m_gameFile->setPlayerRotation((float)m_ui->playerRollSpinBox->value(), (float)m_ui->playerPitchSpinBox->value(), (float)m_ui->playerYawSpinBox->value());
    m_gameFile->setCameraPosition((float)m_ui->cameraXSpinBox->value(),    (float)m_ui->cameraYSpinBox->value(),     (float)m_ui->cameraZSpinBox->value());
    m_gameFile->setCameraRotation((float)m_ui->cameraRollSpinBox->value(), (float)m_ui->cameraPitchSpinBox->value(), (float)m_ui->cameraYawSpinBox->value());
    m_gameFile->setSaveTime(m_ui->saveTimeEdit->dateTime());
    m_gameFile->setNight(m_ui->nightChkbox->isChecked());
    m_gameFile->setHeroMode(m_ui->heroModeChkBox->isChecked());
    m_gameFile->setIntroViewed(m_ui->introViewedChkBox->isChecked());
    m_gameFile->setTotalHP((short)m_ui->totalHPSpinBox->value());
    m_gameFile->setUnkHP((short)m_ui->unkHPSpinBox->value());
    m_gameFile->setCurrentHP((short)m_ui->curHPSpinBox->value());
    m_gameFile->setRoomID((uint)m_ui->roomIDSpinBox->value());
    m_gameFile->setRupees((short)m_ui->rupeeSpinBox->value());
    // Swords
    m_gameFile->setSword(SkywardSwordFile::PracticeSword,  m_ui->practiceSwdChkBox->isChecked());
    m_gameFile->setSword(SkywardSwordFile::GoddessSword,   m_ui->goddessSwdChkBox->isChecked());
    m_gameFile->setSword(SkywardSwordFile::LongSword,      m_ui->longSwdChkBox->isChecked());
    m_gameFile->setSword(SkywardSwordFile::WhiteSword,     m_ui->whiteSwdChkBox->isChecked());
    m_gameFile->setSword(SkywardSwordFile::MasterSword,    m_ui->masterSwdChkBox->isChecked());
    m_gameFile->setSword(SkywardSwordFile::TrueMasterSword,m_ui->trueMasterSwdChkBox->isChecked());
    // Weapons
    m_gameFile->setEquipment(SkywardSwordFile::SlingshotWeapon,      m_ui->slingShotChkBox->isChecked());
    m_gameFile->setEquipment(SkywardSwordFile::ScattershotWeapon,    m_ui->scatterShotChkBox->isChecked());
    m_gameFile->setEquipment(SkywardSwordFile::BugnetWeapon,         m_ui->bugNetChkBox->isChecked());
    m_gameFile->setEquipment(SkywardSwordFile::BigBugnetWeapon,      m_ui->bigBugNetChkBox->isChecked());
    m_gameFile->setEquipment(SkywardSwordFile::BeetleWeapon,         m_ui->beetleChkBox->isChecked());
    m_gameFile->setEquipment(SkywardSwordFile::HookBeetleWeapon,     m_ui->hookBeetleChkBox->isChecked());
    m_gameFile->setEquipment(SkywardSwordFile::QuickBeetleWeapon,    m_ui->quickBeetleChkBox->isChecked());
    m_gameFile->setEquipment(SkywardSwordFile::ToughBeetleWeapon,    m_ui->toughBeetleChkBox->isChecked());
    m_gameFile->setEquipment(SkywardSwordFile::BombWeapon,           m_ui->bombChkBox->isChecked());
    m_gameFile->setEquipment(SkywardSwordFile::GustBellowsWeapon,    m_ui->gustBellowsChkBox->isChecked());
    m_gameFile->setEquipment(SkywardSwordFile::WhipWeapon,           m_ui->whipChkBox->isChecked());
    m_gameFile->setEquipment(SkywardSwordFile::ClawshotWeapon,       m_ui->clawShotChkBox->isChecked());
    m_gameFile->setEquipment(SkywardSwordFile::BowWeapon,            m_ui->bowChkBox->isChecked());
    m_gameFile->setEquipment(SkywardSwordFile::IronBowWeapon,        m_ui->ironBowChkBox->isChecked());
    m_gameFile->setEquipment(SkywardSwordFile::DiggingMittsEquipment,m_ui->diggingMittsChkBox->isChecked());
    m_gameFile->setEquipment(SkywardSwordFile::MoleMittsEquipment,   m_ui->moleMittsChkBox->isChecked());
    m_gameFile->setEquipment(SkywardSwordFile::SacredBowWeapon,      m_ui->sacredBowChkBox->isChecked());
    m_gameFile->setEquipment(SkywardSwordFile::SailClothEquipment,   m_ui->sailClothChkBox->isChecked());
    m_gameFile->setEquipment(SkywardSwordFile::HarpEquipment,        m_ui->harpChkBox->isChecked());
    m_gameFile->setEquipment(SkywardSwordFile::WaterDragonScaleEquipment,m_ui->dragonScaleChkBox->isChecked());
    m_gameFile->setEquipment(SkywardSwordFile::FireShieldEaringsEquipment,m_ui->fireEaringsChkBox->isChecked());
    // Bugs
    m_gameFile->setBug(SkywardSwordFile::HornetBug,     m_ui->hornetChkBox->isChecked());
    m_gameFile->setBug(SkywardSwordFile::ButterflyBug,  m_ui->butterflyChkBox->isChecked());
    m_gameFile->setBug(SkywardSwordFile::DragonflyBug,  m_ui->dragonflyChkBox->isChecked());
    m_gameFile->setBug(SkywardSwordFile::FireflyBug,    m_ui->fireflyChkBox->isChecked());
    m_gameFile->setBug(SkywardSwordFile::RhinoBeetleBug,m_ui->rhinoBeetleChkBox->isChecked());
    m_gameFile->setBug(SkywardSwordFile::LadybugBug,    m_ui->ladybugChkBox->isChecked());
    m_gameFile->setBug(SkywardSwordFile::SandCicadaBug, m_ui->sandCicadaChkBox->isChecked());
    m_gameFile->setBug(SkywardSwordFile::StagBeetleBug, m_ui->stagBeetleChkBox->isChecked());
    m_gameFile->setBug(SkywardSwordFile::GrasshopperBug,m_ui->grasshopperChkBox->isChecked());
    m_gameFile->setBug(SkywardSwordFile::MantisBug,     m_ui->mantisChkBox->isChecked());
    m_gameFile->setBug(SkywardSwordFile::AntBug,        m_ui->antChkBox->isChecked());
    m_gameFile->setBug(SkywardSwordFile::RollerBug,     m_ui->eldinRollerChkBox->isChecked());
    // Materials
    m_gameFile->setMaterial(SkywardSwordFile::HornetLarvaeMaterial,   m_ui->hornetLarvaeChkBox->isChecked());
    m_gameFile->setMaterial(SkywardSwordFile::BirdFeatherMaterial,    m_ui->birdFeatherChkBox->isChecked());
    m_gameFile->setMaterial(SkywardSwordFile::TumbleWeedMaterial,     m_ui->tumbleWeedChkBox->isChecked());
    m_gameFile->setMaterial(SkywardSwordFile::LizardTailMaterial,     m_ui->lizardTailChkBox->isChecked());
    m_gameFile->setMaterial(SkywardSwordFile::EldinOreMaterial,       m_ui->eldinOreChkBox->isChecked());
    m_gameFile->setMaterial(SkywardSwordFile::AncientFlowerMaterial,  m_ui->ancientFlowerChkBox->isChecked());
    m_gameFile->setMaterial(SkywardSwordFile::AmberRelicMaterial,     m_ui->amberRelicChkBox->isChecked());
    m_gameFile->setMaterial(SkywardSwordFile::DuskRelicMaterial,      m_ui->duskRelicChkBox->isChecked());
    m_gameFile->setMaterial(SkywardSwordFile::JellyBlobMaterial,      m_ui->jellyBlobChkBox->isChecked());
    m_gameFile->setMaterial(SkywardSwordFile::MonsterClawMaterial,    m_ui->monsterClawChkBox->isChecked());
    m_gameFile->setMaterial(SkywardSwordFile::MonsterHornMaterial,    m_ui->monsterHornChkBox->isChecked());
    m_gameFile->setMaterial(SkywardSwordFile::OrnamentalSkullMaterial,m_ui->decoSkullChkBox->isChecked());
    m_gameFile->setMaterial(SkywardSwordFile::EvilCrystalMaterial,    m_ui->evilCrystalChkBox->isChecked());
    m_gameFile->setMaterial(SkywardSwordFile::BlueBirdFeatherMaterial,m_ui->blueBirdFeatherChkBox->isChecked());
    m_gameFile->setMaterial(SkywardSwordFile::GoldenSkullMaterial,    m_ui->goldenSkullChkBox->isChecked());
    m_gameFile->setMaterial(SkywardSwordFile::GoddessPlumeMaterial,   m_ui->goddessPlumeChkBox->isChecked());

    // Amounts
    // Ammo
    m_gameFile->setAmmo(SkywardSwordFile::ArrowAmmo, m_ui->arrowAmmoSpinBox->value());
    m_gameFile->setAmmo(SkywardSwordFile::BombAmmo,  m_ui->bombAmmoSpinBox->value());
    m_gameFile->setAmmo(SkywardSwordFile::SeedAmmo,  m_ui->seedAmmoSpinBox->value());
    // Bugs
    m_gameFile->setBugQuantity(SkywardSwordFile::HornetBug,     m_ui->hornetSpinBox->value());
    m_gameFile->setBugQuantity(SkywardSwordFile::ButterflyBug,  m_ui->butterflySpinBox->value());
    m_gameFile->setBugQuantity(SkywardSwordFile::DragonflyBug,  m_ui->dragonflySpinBox->value());
    m_gameFile->setBugQuantity(SkywardSwordFile::FireflyBug,    m_ui->fireflySpinBox->value());
    m_gameFile->setBugQuantity(SkywardSwordFile::RhinoBeetleBug,m_ui->rhinoBeetleSpinBox->value());
    m_gameFile->setBugQuantity(SkywardSwordFile::LadybugBug,    m_ui->ladybugSpinBox->value());
    m_gameFile->setBugQuantity(SkywardSwordFile::SandCicadaBug, m_ui->sandCicadaSpinBox->value());
    m_gameFile->setBugQuantity(SkywardSwordFile::StagBeetleBug, m_ui->stagBeetleSpinBox->value());
    m_gameFile->setBugQuantity(SkywardSwordFile::GrasshopperBug,m_ui->grasshopperSpinBox->value());
    m_gameFile->setBugQuantity(SkywardSwordFile::MantisBug,     m_ui->mantisSpinBox->value());
    m_gameFile->setBugQuantity(SkywardSwordFile::AntBug,        m_ui->antSpinBox->value());
    m_gameFile->setBugQuantity(SkywardSwordFile::RollerBug,     m_ui->eldinRollerSpinBox->value());
    // Materials
    m_gameFile->setMaterialQuantity(SkywardSwordFile::HornetLarvaeMaterial,   m_ui->hornetLarvaeSpinBox->value());
    m_gameFile->setMaterialQuantity(SkywardSwordFile::BirdFeatherMaterial,    m_ui->birdFeatherSpinBox->value());
    m_gameFile->setMaterialQuantity(SkywardSwordFile::TumbleWeedMaterial,     m_ui->tumbleWeedSpinBox->value());
    m_gameFile->setMaterialQuantity(SkywardSwordFile::LizardTailMaterial,     m_ui->lizardTailSpinBox->value());
    m_gameFile->setMaterialQuantity(SkywardSwordFile::EldinOreMaterial,       m_ui->eldinOreSpinBox->value());
    m_gameFile->setMaterialQuantity(SkywardSwordFile::AncientFlowerMaterial,  m_ui->ancientFlowerSpinBox->value());
    m_gameFile->setMaterialQuantity(SkywardSwordFile::AmberRelicMaterial,     m_ui->amberRelicSpinBox->value());
    m_gameFile->setMaterialQuantity(SkywardSwordFile::DuskRelicMaterial,      m_ui->duskRelicSpinBox->value());
    m_gameFile->setMaterialQuantity(SkywardSwordFile::JellyBlobMaterial,      m_ui->jellyBlobSpinBox->value());
    m_gameFile->setMaterialQuantity(SkywardSwordFile::MonsterClawMaterial,    m_ui->monsterClawSpinBox->value());
    m_gameFile->setMaterialQuantity(SkywardSwordFile::MonsterHornMaterial,    m_ui->monsterHornSpinBox->value());
    m_gameFile->setMaterialQuantity(SkywardSwordFile::OrnamentalSkullMaterial,m_ui->decoSkullSpinBox->value());
    m_gameFile->setMaterialQuantity(SkywardSwordFile::EvilCrystalMaterial,    m_ui->evilCrystalSpinBox->value());
    m_gameFile->setMaterialQuantity(SkywardSwordFile::BlueBirdFeatherMaterial,m_ui->blueBirdFeatherSpinBox->value());
    m_gameFile->setMaterialQuantity(SkywardSwordFile::GoldenSkullMaterial,    m_ui->goldenSkullSpinBox->value());
    m_gameFile->setMaterialQuantity(SkywardSwordFile::GoddessPlumeMaterial,   m_ui->goddessPlumeSpinBox->value());

    m_gameFile->setGratitudeCrystalAmount(m_ui->gratitudeCrystalSpinBox->value());
    m_gameFile->updateChecksum();
    updateTitle();
    updateInfo();
}

void MainWindow::onOpen()
{
    QFileDialog fileDialog;
    QString filename = fileDialog.getOpenFileName(this, tr("Open Skyward Sword Save..."), dir, tr("Skyward Sword Save Files (*.sav);;SaveData (*.bin)"));
    if (!filename.isEmpty())
    {
        if (m_gameFile)
            delete m_gameFile;

        m_gameFile = new SkywardSwordFile;

        if (filename.lastIndexOf(".bin") == filename.size() - 4)
        {
            if (!m_gameFile->loadDataBin(filename, m_gameFile->game()))
                return;
        }
        else if (!m_gameFile->open(m_gameFile->game(), filename))
            return;

        m_gameFile->setGame(SkywardSwordFile::Game1);
        m_ui->actionGame1->setChecked(true);

        if (!m_gameFile->hasValidChecksum())
        {
            QMessageBox msg(QMessageBox::Warning, tr("CRC32 Mismatch"), tr("The checksum generated does not match the one provided by the file"));
            msg.exec();
        }

        m_ui->menuRecent->addAction(m_gameFile->filename());

        foreach(QString file, m_fileWatcher->files())
            m_fileWatcher->removePath(file);


        m_fileWatcher->addPath(filename);
        m_hexEdit->setData(m_gameFile->gameData());
        updateInfo();
        updateTitle();
        updateMRU();
    }
}

void MainWindow::onNew()
{
   /* if (!m_gameFile)
        m_gameFile = new SkywardSwordFile();

    m_gameFile->open();
    toggleWidgetStates();
    updateTitle();
    updateInfo();*/

    if (!m_newFileDialog)
        m_newFileDialog = new NewFileDialog(this);

    int res = m_newFileDialog->exec();

    if (res == QDialog::Accepted)
    {
        bool fileValid = false;
        if (!m_gameFile)
            m_gameFile = new SkywardSwordFile((SkywardSwordFile::Region)m_newFileDialog->region());

        for (quint32 i = 0; i < IGameFile::GameCount; i++)
        {
            if (m_newFileDialog->isGameValid(i))
            {
                fileValid = true;
                m_gameFile->createNewGame((IGameFile::Game)i);
                m_gameFile->setNew(false);
                m_gameFile->setPlayerName(m_newFileDialog->playerName     (i));
                m_gameFile->setRupees    (m_newFileDialog->rupees         (i));
                m_gameFile->setCurrentHP (m_newFileDialog->currentHealth  (i));
                m_gameFile->setTotalHP   (m_newFileDialog->heartContainers(i) * 4);
                m_gameFile->updateChecksum();
            }
            else
            {
                qDebug() << "invalid";
                continue;
            }
        }

        if (fileValid)
        {
            updateTitle();
            toggleWidgetStates();
            updateInfo();
        }
        else
        {
            delete m_gameFile;
            m_gameFile = NULL;
        }
    }
}

void MainWindow::onCreateNewGame()
{
    if (!m_gameFile)
        m_gameFile = new SkywardSwordFile();

    NewGameDialog* ngd = new NewGameDialog(this, m_curGame);
    ngd->setWindowTitle("New Adventure...");
    ngd->exec();
    if (ngd->result() == NewGameDialog::Accepted)
    {
        ngd->gameFile(*m_gameFile);

        updateInfo();
        updateTitle();
        m_hexEdit->setData(m_gameFile->gameData());
    }
    delete ngd;
}

void MainWindow::onDeleteGame()
{
    if (!m_gameFile || !m_gameFile->isOpen())
                 return;

    m_gameFile->deleteGame(m_curGame);
    m_ui->tabWidget->setCurrentIndex(0);
    clearInfo();
    updateInfo();
    updateTitle();
    m_hexEdit->setData(m_gameFile->gameData());
}

void MainWindow::onSave()
{
    if (!m_gameFile)
        return;

    QString oldFilename = m_gameFile->filename();
    foreach(QString file, m_fileWatcher->files())
        m_fileWatcher->removePath(file);

    if (!m_gameFile || !m_gameFile->isOpen() || m_gameFile->game() == SkywardSwordFile::GameNone)
        return;

    if (m_gameFile->filename().size() <= 0)
    {
        QFileDialog fileDialog;
        QString file = fileDialog.getSaveFileName(this, tr("Save Skyward Sword Save File..."), dir, tr("Skyward Sword Save Files (*.sav);;Wii save (*.bin)"));
        m_gameFile->setFilename(file);
    }

    if(m_gameFile->save())
    {
        m_ui->statusBar->showMessage(tr("Save successful!"));
    }
    else
    {
        if (!m_oldFilename.isEmpty())
        {
            m_gameFile->setFilename(m_oldFilename);
            m_oldFilename = "";
        }
        m_ui->statusBar->showMessage(tr("Unable to save file"));
    }

    m_fileWatcher->addPath(m_gameFile->filename());
    m_gameFile->updateChecksum();
    updateInfo();
    updateTitle();
    m_hexEdit->setData(m_gameFile->gameData());
    if (oldFilename != m_gameFile->filename())
    {
        m_hexEdit->undoStack()->clear();
        m_hexEdit->setCursorPosition(0);
        m_hexEdit->update();
    }
}

void MainWindow::onSaveAs()
{
    if (!m_gameFile)
        return;

    m_oldFilename = m_gameFile->filename();
    m_gameFile->setFilename(QString(tr("")));
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

    m_gameFile->updateChecksum();
    updateInfo();
    updateTitle();
    m_hexEdit->setData(m_gameFile->gameData());
}

void MainWindow::onPreferences()
{
    PreferencesDialog prefDiag(this);
    quint32 result = prefDiag.exec();
    switch(result)
    {
    case QDialog::Accepted:
        WiiKeys::instance()->saveKeys();
        break;
    default:
        break;
    }
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

    m_gameFile->setGame((SkywardSwordFile::Game)m_curGame);
    updateInfo();
    updateTitle();
    m_hexEdit->setData(m_gameFile->gameData());
}

void MainWindow::onFileChanged(QString file)
{
    m_fileWatcher->disconnect();
    QMessageBox msg(QMessageBox::Information, "File Changed", tr("File at location:\n\"%1\"\nHas been modified, do you wish to reload?").arg(file), QMessageBox::Ok | QMessageBox::Ignore, this);
    int res = msg.exec();

    if (res == QMessageBox::Ok)
    {
        m_fileWatcher->removePath(file);
        if (m_gameFile->reload(m_gameFile->game()))
        {
            m_fileWatcher->addPath(file);
            m_gameFile->updateChecksum();
        }
        updateTitle();
        updateInfo();
        m_hexEdit->setData(m_gameFile->gameData());
    }

    connect(m_fileWatcher, SIGNAL(fileChanged(QString)), this, SLOT(onFileChanged(QString)));
}

void MainWindow::onReload()
{
    if (!m_gameFile || !m_gameFile->isOpen())
        return;

    foreach (QString file, m_fileWatcher->files())
        m_fileWatcher->removePath(file);

    m_gameFile->reload(m_gameFile->game());
    if(m_gameFile->isOpen())
    {
        updateInfo();
        m_ui->statusBar->showMessage(tr("File successfully reloaded"));
        m_fileWatcher->addPath(m_gameFile->filename());

        updateTitle();
        m_hexEdit->setData(m_gameFile->gameData());

    }
    else
    {
        clearInfo();
        m_ui->statusBar->showMessage(tr("Unable to reload file, is it still there?"));
    }
}

void MainWindow::onClose()
{
    if (!m_oldFilename.isEmpty() && m_fileWatcher->files().contains(m_oldFilename))
        m_fileWatcher->removePath(m_oldFilename);

    if (!m_gameFile || !m_gameFile->isOpen())
                 return;
    if(m_gameFile->isModified())
    {
        QString filename = QFileInfo(m_gameFile->filename()).fileName();
        QMessageBox msg(QMessageBox::Information,
                        "File Modified",
                        QString(tr("The file \"%1\" has been modified.\n Do you wish to save?"))
                        .arg(filename.isEmpty() ? tr("Untitled") : filename),
                        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        int result = msg.exec();
        if (result == QMessageBox::Yes)
           onSave();

        if(result == QMessageBox::Cancel)
           return;
    }

    m_fileWatcher->removePath(m_gameFile->filename());
    m_gameFile->close();
    m_hexEdit->setData(m_gameFile->gameData());
    delete m_gameFile;
    m_gameFile = NULL;

    clearInfo();
    m_ui->tabWidget->setEnabled(false);

    m_ui->createDeleteGameBtn->setText(tr("Click to create a new Adventure"));
    if (m_ui->createDeleteGameBtn->disconnect())
                 connect(m_ui->createDeleteGameBtn, SIGNAL(clicked()), this, SLOT(onCreateNewGame()));

    updateTitle();
    updateInfo();
}

void MainWindow::toggleWidgetStates()
{
    m_ui->actionFileInfo->setEnabled((m_gameFile != NULL && m_gameFile->isOpen()));
    m_ui->tabWidget->setVisible   (m_gameFile != NULL && m_gameFile->isOpen());
    m_ui->actionSave->setEnabled  (m_gameFile != NULL && m_gameFile->isOpen());
    m_ui->actionSaveAs->setEnabled(m_gameFile != NULL && m_gameFile->isOpen());
    m_ui->actionClose->setEnabled (m_gameFile != NULL && m_gameFile->isOpen());
    m_ui->actionReload->setEnabled(m_gameFile != NULL && m_gameFile->isOpen());
    for (int i = 0; i < SkywardSwordFile::GameCount; i++)
    {
        QAction* act = this->findChild<QAction*>(QString("actionGame%1").arg(i+1));
        if (act)
            act->setEnabled(m_gameFile != NULL && m_gameFile->isOpen());
        else
            qDebug() << "Could not find specified action!";
    }
}

void MainWindow::updateMRU()
{
    QSettings settings;
    settings.beginGroup("MRU");
    QStringList files = settings.value("files", "").toString().split("?");
    if (files.contains(m_gameFile->filename()))
    {
        files.removeAt(files.indexOf(m_gameFile->filename()));
        files.push_front(m_gameFile->filename());
    }
    else
        files.push_front(m_gameFile->filename());

    files.removeDuplicates(); // Make sure there are no duplicates
    settings.setValue("files", files.join("?"));
    settings.endGroup();
}

void MainWindow::updateTitle()
{
    if (m_gameFile == NULL || !m_gameFile->isOpen() || m_gameFile->game() == SkywardSwordFile::GameNone)
                 this->setWindowTitle(tr("WiiKing2 Editor"));
    else
    {
        QFileInfo fileInfo(m_gameFile->filename());
        //HACK: Does this count as a hack?
        if (fileInfo.fileName().isEmpty())
            fileInfo.setFile(QDir(), "Untitled");

        this->setWindowTitle(QString(tr("WiiKing2 Editor (%1%2) - Game %3 0x"))
                            .arg(fileInfo.fileName())
                            .arg((m_gameFile->isModified() ? "*" : ""))
                            .arg(m_gameFile->game() + 1)
                            .append(QString("").sprintf("%08X", m_gameFile->checksum())));
    }
}

void MainWindow::clearInfo()
{
    m_isUpdating = true;

    foreach (QLineEdit* widget, findChildren<QLineEdit*>())
    {
        widget->clear();
    }

    foreach(QSpinBox* widget, findChildren<QSpinBox*>())
    {
        widget->clear();
    }

    foreach(QCheckBox* widget, findChildren<QCheckBox*>())
    {
        widget->setChecked(false);
    }

    foreach(QDateTimeEdit* widget, findChildren<QDateTimeEdit*>())
    {
        widget->clear();
    }

    foreach(QHexEdit* widget, findChildren<QHexEdit*>())
    {
        widget->setData(QByteArray(0x53BC, 0));
    }

    m_isUpdating = false;
}



void MainWindow::closeEvent(QCloseEvent* e)
{
    if (m_gameFile && m_gameFile->isModified())
    {
        QString filename = (m_gameFile->filename().isEmpty() ? "Untitled" : QFileInfo(m_gameFile->filename()).fileName());
        QMessageBox msg(QMessageBox::Question, "Confirm?", QString("The file \"<b>%1</b>\" has been modified.\nDo you wish to save?").arg(filename), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        quint32 result = msg.exec();

        switch(result)
        {
        case QMessageBox::Yes:
            onSave();
            e->accept();
            break;
        case QMessageBox::No:
            e->accept();
            break;
        case QMessageBox::Cancel:
            e->ignore();
        default:
            e->accept();
        }
    }

    e->accept();
}
