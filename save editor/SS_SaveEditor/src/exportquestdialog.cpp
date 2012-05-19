#include "exportquestdialog.h"
#include "ui_exportquestdialog.h"

ExportQuestDialog::ExportQuestDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExportQuestDialog)
{
    ui->setupUi(this);
}

ExportQuestDialog::~ExportQuestDialog()
{
    delete ui;
}
