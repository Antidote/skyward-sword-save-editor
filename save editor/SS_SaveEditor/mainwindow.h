#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "gamefile.h"
#include <QMainWindow>

class QActionGroup;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
    void UpdateInfo();
    void ClearInfo();
private slots:
    void onTextChanged(QString text);
    void onValueChanged();
    void onGameChanged(QAction*);
    void onOpen();
    void onSave();
    void onSaveAs();
    void onReload();
    void onClose();
private:
    void ToggleVisible(bool visible);
    void SetupActions();
    void SetupConnections();
    Ui::MainWindow   *m_ui;
    GameFile*         m_gameFile;
    GameFile::Game    m_curGame;
    bool              m_isUpdating;

    QActionGroup* m_gameGroup;
};

#endif // MAINWINDOW_H
