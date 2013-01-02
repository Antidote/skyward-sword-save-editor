#ifndef NEWFILEDIALOG_H
#define NEWFILEDIALOG_H

#include <QDialog>
class QToolBox;

namespace Ui {
class NewFileDialog;
}

class NewFileDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit NewFileDialog(QWidget *parent = 0);
    ~NewFileDialog();

    QString playerName() const;
    
private:
    Ui::NewFileDialog *m_ui;
    QToolBox* m_gameToolBox;
};

#endif // NEWFILEDIALOG_H
