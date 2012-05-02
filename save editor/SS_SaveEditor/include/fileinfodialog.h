#ifndef FILEINFODIALOG_H
#define FILEINFODIALOG_H

#include <QDialog>

class GameFile;
class QAbstractButton;

namespace Ui {
    class FileInfoDialog;
}

class FileInfoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FileInfoDialog(QWidget *parent, GameFile& game);
    ~FileInfoDialog();

private slots:
    void onRegionChanged(QAbstractButton*);
private:
    Ui::FileInfoDialog *m_ui;
    GameFile* m_gameFile;
};

#endif // FILEINFODIALOG_H
