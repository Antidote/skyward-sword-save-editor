#include "gameinfowidget.h"
#include "ui_gameinfowidget.h"

GameInfoWidget::GameInfoWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GameInfoWidget)
{
    ui->setupUi(this);
}

GameInfoWidget::~GameInfoWidget()
{
    delete ui;
}
