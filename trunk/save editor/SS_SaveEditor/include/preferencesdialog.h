#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QDialog>

namespace Ui {
class PreferencesDialog;
}

class PreferencesDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit PreferencesDialog(QWidget *parent = 0);
    ~PreferencesDialog();

public slots:
    void accept();
    void reject();

private slots:
    void onTextChanged(QString);

private:
    Ui::PreferencesDialog *m_ui;
};

#endif // PREFERENCES_H
