#include "newfiledialog.h"
#include "ui_newfiledialog.h"
#include <QToolBox>
#include <QLineEdit>

NewFileDialog::NewFileDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::NewFileDialog)
{
    m_ui->setupUi(this);
}

QString NewFileDialog::playerName() const
{
    QLineEdit* temp = ((QLineEdit*)(m_ui->gameToolBox->currentWidget()->findChild<QLineEdit*>("playerNameLineEdit")));
    if (!temp)
        return QString();
    QString name = temp->text();
    return name;
}

NewFileDialog::~NewFileDialog()
{
    delete m_ui;
}
