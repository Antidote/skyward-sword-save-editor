#ifndef NEWFILEDIALOG_H
#define NEWFILEDIALOG_H

#include <QDialog>
class QToolBox;

namespace Ui {
class NewFileDialog;
}

class GameInfoWidget;

class NewFileDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit NewFileDialog(QWidget *parent = 0);
    ~NewFileDialog();

    QString playerName() const;
    
private:
    Ui::NewFileDialog *m_ui;
    QToolBox*       m_gameToolBox;
    GameInfoWidget* m_game1Widget;
    GameInfoWidget* m_game2Widget;
    GameInfoWidget* m_game3Widget;
};

#endif // NEWFILEDIALOG_H
