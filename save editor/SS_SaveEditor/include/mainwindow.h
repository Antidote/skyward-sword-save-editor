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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "skywardswordfile.h"
#include <QMainWindow>
#include <QDateTime>
#include <QTimer>


class QFileSystemWatcher;
class QActionGroup;
class QButtonGroup;
class QAbstractButton;
class QHexEdit;
class NewFileDialog;
class SettingsManager;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    static const quint32 UPDATE_DELAY = 5000;
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
    void updateInfo();
    void updateTitle(); // any reason these are exposed?
    void clearInfo();
    SkywardSwordFile* gameFile();

private slots:
    void onTextChanged(QString text);
    void onValueChanged();
    void onGameChanged(QAction*);
    void onCreateNewGame();
    void onDeleteGame();
    void onOpen();
    void onNew();
    void onSave();
    void onSaveAs();
    void onAbout();
    void onAboutQt();
    void onReload();
    void onClose();
    void onFileInfo();
    void onPreferences();
    void onFileChanged(QString);
    void onCurrentAdressChanged(int);
    void onHexDataChanged();
    void onHexGotoAddress();
    void closeEvent(QCloseEvent* e);

private:
    bool askOnClose();
    void updateMRU();
    void toggleWidgetStates();
    void dragEnterEvent(QDragEnterEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);
    QTimer* m_checkTimer;
    void setRegion(SkywardSwordFile::Region);
    void setupHexEdit();
    void setupActions();
    void setupConnections();
    Ui::MainWindow*           m_ui;
    QString                   m_oldFilename;
    SkywardSwordFile*         m_gameFile;
    SkywardSwordFile::Game    m_curGame;
    bool                      m_isUpdating;
    bool                      m_isChecking;
    QActionGroup*             m_gameGroup;
    QButtonGroup*             m_regionGroup;
    QFileSystemWatcher*       m_fileWatcher;
    QHexEdit*                 m_hexEdit;
    NewFileDialog*            m_newFileDialog;
    SettingsManager*          m_settingsManager;
};

#endif // MAINWINDOW_H
