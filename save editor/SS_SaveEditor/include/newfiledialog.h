#ifndef NEWFILEDIALOG_H
#define NEWFILEDIALOG_H

#include <QDialog>
class QToolBox;
class QAbstractButton;
class GameInfoWidget;

namespace Ui {
class NewFileDialog;
}

class NewFileDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit NewFileDialog(QWidget *parent = 0);
    ~NewFileDialog();

    quint32 region         ()                   const;
    bool    isGameValid    (const quint32 game) const;
    QString playerName     (const quint32 game) const;
    quint32 rupees         (const quint32 game) const;
    quint32 heartContainers(const quint32 game) const;
    quint32 currentHealth  (const quint32 game) const;


private slots:
    void showEvent(QShowEvent *);
    void onButtonClicked(QAbstractButton*);
private:
    Ui::NewFileDialog *m_ui;
    QToolBox*       m_gameToolBox;
    GameInfoWidget* m_game1Widget;
    GameInfoWidget* m_game2Widget;
    GameInfoWidget* m_game3Widget;
};

#endif // NEWFILEDIALOG_H
