#ifndef MENUBAR_H
#define MENUBAR_H

#include <QMenuBar>

class QMenu;
class QToolBar;
class FileMenu;

class MenuBar : public QMenuBar
{
    Q_OBJECT
public:
    static MenuBar* instance();
    static QToolBar* toolBar();
signals:
protected:
private:
    // Disable creating an instance
    MenuBar();
    MenuBar(const MenuBar&) {}
    MenuBar& operator=(const MenuBar&){}

    FileMenu* m_fileMenu;

    static QToolBar* m_toolBar;
    static MenuBar* m_instance;
public slots:
protected slots:
private slots:
};

#endif // MENUBAR_H
