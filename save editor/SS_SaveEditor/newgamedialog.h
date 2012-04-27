#ifndef NEWGAMEDIALOG_H
#define NEWGAMEDIALOG_H

#include <QDialog>
#include "gamefile.h"

class QAbstractButton;

namespace Ui {
class NewGameDialog;
}

class NewGameDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit NewGameDialog(QWidget *parent = 0, GameFile::Game game = GameFile::Game1);
    ~NewGameDialog();

    GameFile* gameFile(GameFile* gameFile = NULL);

private slots:
    void onRegionChanged(QAbstractButton*);
private:
    Ui::NewGameDialog* m_ui;
    GameFile::Game     m_game;
};

#endif // NEWGAMEDIALOG_H
