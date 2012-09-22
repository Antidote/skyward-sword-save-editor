#include "menus/menubar.h"
#include "menus/filemenu.h"
// Qt Includes
#include <QMenuBar>
#include <QToolBar>

MenuBar* MenuBar::m_instance = NULL;
QToolBar* MenuBar::m_toolBar = NULL;

MenuBar::MenuBar()
{
    m_toolBar = new QToolBar;
    m_toolBar->setAllowedAreas(Qt::TopToolBarArea);
    m_toolBar->setMovable(false);

    m_fileMenu = new FileMenu(this);
    this->addMenu(m_fileMenu);
}


QToolBar* MenuBar::toolBar()
{
    if (!m_toolBar)
        m_toolBar = new QToolBar;

    return m_toolBar;
}

MenuBar* MenuBar::instance()
{
    if (!m_instance)
        m_instance = new MenuBar;

    return m_instance;
}
