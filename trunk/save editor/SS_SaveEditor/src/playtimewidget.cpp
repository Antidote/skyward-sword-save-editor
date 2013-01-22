#include "playtimewidget.h"
#include "ui_playtimewidget.h"
#include "skywardswordfile.h"

#include <QDebug>

PlayTimeWidget::PlayTimeWidget(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::PlayTimeWidget),
    m_blink(false)
{
    m_ui->setupUi(this);

    m_blinkTimer = new QTimer(this);
    m_blinkTimer->setInterval(1000); // Blink every 2 seconds (needs to be tweaked)
    m_blinkTimer->start();
    connect(m_blinkTimer, SIGNAL(timeout()), this, SLOT(blinkText()));
    connect(m_ui->daysSpinBox,  SIGNAL(valueChanged(int)), this, SLOT(valueChanged()));
    connect(m_ui->hoursSpinBox, SIGNAL(valueChanged(int)), this, SLOT(valueChanged()));
    connect(m_ui->minsSpinBox,  SIGNAL(valueChanged(int)), this, SLOT(valueChanged()));
    connect(m_ui->secsSpinBox,  SIGNAL(valueChanged(int)), this, SLOT(valueChanged()));
}

PlayTimeWidget::~PlayTimeWidget()
{
    delete m_ui;
}

void PlayTimeWidget::setPlayTime(PlayTime val)
{
    m_ui->daysSpinBox ->setValue(val.Days);
    m_ui->hoursSpinBox->setValue(val.Hours);
    m_ui->minsSpinBox ->setValue(val.Minutes);
    m_ui->secsSpinBox ->setValue(val.Seconds);
}

void PlayTimeWidget::blinkText()
{
    m_blink ^= 1; // Toggle blink

    if (this->isEnabled())
    {
        m_ui->hourSepLbl->setProperty("blink", m_blink);
        m_ui->minSepLbl->setProperty("blink", m_blink);
        m_ui->secSepLbl->setProperty("blink", m_blink);
        style()->unpolish(m_ui->hourSepLbl);
        style()->polish(m_ui->hourSepLbl);
        style()->unpolish(m_ui->minSepLbl);
        style()->polish(m_ui->minSepLbl);
        style()->unpolish(m_ui->secSepLbl);
        style()->polish(m_ui->secSepLbl);
    }

    m_blinkTimer->start((m_blink ? 500 : 1000));
}

void PlayTimeWidget::valueChanged()
{
    emit playTimeChanged((PlayTime){m_ui->daysSpinBox->value(), m_ui->hoursSpinBox->value(),
                                    m_ui->minsSpinBox->value(), m_ui->secsSpinBox->value()});
}
