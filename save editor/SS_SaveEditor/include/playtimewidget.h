#ifndef PLAYTIMEWIDGET_H
#define PLAYTIMEWIDGET_H

#include <QWidget>

#include <QTimer>

namespace Ui {
class PlayTimeWidget;
}

#include "skywardswordfile.h"

class PlayTimeWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit PlayTimeWidget(QWidget *parent = 0);
    ~PlayTimeWidget();

    void setPlayTime(PlayTime);

signals:
    void playTimeChanged(PlayTime);

private slots:
    void blinkText();
    void valueChanged();

private:
    Ui::PlayTimeWidget *m_ui;
    QTimer             *m_blinkTimer;
    bool                m_blink;
};

#endif // PLAYTIMEWIDGET_H
