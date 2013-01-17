#ifndef GAMEINFOWIDGET_H
#define GAMEINFOWIDGET_H

#include <QWidget>

namespace Ui {
class GameInfoWidget;
}

class GameInfoWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit GameInfoWidget(QWidget *parent = 0);
    ~GameInfoWidget();
    
    bool    isGameValid    () const;
    void   setGameValid    (const bool val);
    QString playerName     () const;
    void setPlayerName(const QString&);
    quint32 rupees         () const;
    quint32 heartContainers() const;
    quint32 currentHealth  () const;
private:
    Ui::GameInfoWidget *m_ui;
};

#endif // GAMEINFOWIDGET_H
