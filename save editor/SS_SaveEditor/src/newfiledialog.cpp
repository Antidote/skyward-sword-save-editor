#include "newfiledialog.h"
#include "ui_newfiledialog.h"
#include "gameinfowidget.h"
#include <QToolBox>
#include <QLineEdit>

NewFileDialog::NewFileDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::NewFileDialog)
{
    m_ui->setupUi(this);
    m_gameToolBox = new QToolBox(this);
    m_game1Widget = new GameInfoWidget();
    m_gameToolBox->addItem(m_game1Widget, "Game 1");
    m_game2Widget = new GameInfoWidget();
    m_gameToolBox->addItem(m_game2Widget, "Game 2");
    m_game3Widget = new GameInfoWidget();
    m_gameToolBox->addItem(m_game3Widget, "Game 3");
    m_ui->toolBoxLayout->addWidget(m_gameToolBox);
}

NewFileDialog::~NewFileDialog()
{
    delete m_ui;
}


QString NewFileDialog::playerName() const
{
    QLineEdit* temp = ((QLineEdit*)(m_gameToolBox->currentWidget()->findChild<QLineEdit*>("playerNameLineEdit")));
    if (!temp)
        return QString();
    QString name = temp->text();
    return name;
}
