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

#include "gamefile.h"
#include <QMainWindow>
#include <QDateTime>
#include <QTimer>

class QActionGroup;
class QButtonGroup;
class QAbstractButton;

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
    
    void UpdateInfo();
    void UpdateTitle(); // any reason these are exposed?
    void ClearInfo();

private slots:
    void onDateTimeChanged(QDateTime);
    void onTextChanged(QString text);
    void onValueChanged();
    void onGameChanged(QAction*);
    void onCreateNewGame();
    void onDeleteGame();
    void onOpen();
    void onSave();
    void onSaveAs();
    void onAbout();
    void onAboutQt();
    void onReload();
    void onClose();
    void onFileInfo();
    void onCheck(); // Checks for changes to file and prompts for an update.

private:
    bool event(QEvent *);
    QTimer* m_checkTimer;
    void SetRegion(GameFile::Region);
    void ToggleEnabled(bool visible);
    void SetupActions();
    void SetupConnections();
    Ui::MainWindow*   m_ui;
    GameFile*         m_gameFile;
    GameFile::Game    m_curGame;
    bool              m_isUpdating;

    QActionGroup* m_gameGroup;
    QButtonGroup* m_regionGroup;
};

#endif // MAINWINDOW_H
