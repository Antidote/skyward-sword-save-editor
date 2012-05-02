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
