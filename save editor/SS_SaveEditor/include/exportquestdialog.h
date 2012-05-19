#ifndef EXPORTQUESTDIALOG_H
#define EXPORTQUESTDIALOG_H

#include <QDialog>

namespace Ui {
class ExportQuestDialog;
}

class ExportQuestDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit ExportQuestDialog(QWidget *parent = 0);
    ~ExportQuestDialog();
    
private:
    Ui::ExportQuestDialog *ui;
};

#endif // EXPORTQUESTDIALOG_H
