#ifndef NEWGAMEDIALOG_H
#define NEWGAMEDIALOG_H

#include <QDialog>

class GameFile;
namespace Ui {
class NewGameDialog;
}

class NewGameDialog : public QDialog
{
    Q_OBJECT
    
public:
    // Default coords for new file
    static const float DEFAULT_POS_X = -4798.150391;
    static const float DEFAULT_POS_Y =  1237.629517;
    static const float DEFAULT_POS_Z = -6573.722656;
    explicit NewGameDialog(QWidget *parent = 0);
    ~NewGameDialog();

    GameFile* gameFile();
    
private:
    Ui::NewGameDialog* m_ui;
    GameFile* m_gameFile;
};

#endif // NEWGAMEDIALOG_H
