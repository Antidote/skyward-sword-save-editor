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
    
private:
    Ui::GameInfoWidget *ui;
};

#endif // GAMEINFOWIDGET_H
