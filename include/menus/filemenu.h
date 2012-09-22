#ifndef FILEMENU_H
#define FILEMENU_H

#include <QMenu>

class QAction;
class FileMenu : public QMenu
{
    Q_OBJECT
public:
    FileMenu(QWidget* parent = 0);

private:
    QAction* m_actionOpen;
    QAction* m_actionNew;
    // Separator
    QAction* m_actionSave;
    QAction* m_actionSaveAs;
    // Separator
    QAction* m_actionClose;
    QAction* m_actionExport;
    // Separator
    QAction* m_actionExit;
};

#endif // FILEMENU_H
