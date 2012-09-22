#include "menus/filemenu.h"
#include "menus/menubar.h"

#include <QApplication>
#include <QAction>
#include <QToolBar>

FileMenu::FileMenu(QWidget* parent) :
    QMenu(parent)
{
    this->setTitle(tr("&File"));

    m_actionOpen = MenuBar::toolBar()->addAction("&Open");
    m_actionOpen->setShortcut(QKeySequence::Open);
    m_actionOpen->setIcon(QIcon(":/Icons/Open"));
    this->addAction(m_actionOpen);

    m_actionNew = MenuBar::toolBar()->addAction(tr("&New"));
    m_actionNew->setShortcut(QKeySequence::New);
    m_actionNew->setIcon(QIcon(":/Icons/New"));
    this->addAction(m_actionNew);

    MenuBar::toolBar()->addSeparator();
    this->addSeparator();

    m_actionSave = MenuBar::toolBar()->addAction(tr("&Save"));
    m_actionSave->setShortcut(QKeySequence::Save);
    m_actionSave->setIcon(QIcon(":/Icons/Save"));
    this->addAction(m_actionSave);

    m_actionSaveAs = this->addAction(tr("Save &As"));
    m_actionSaveAs->setShortcut(QKeySequence::SaveAs);
    m_actionSaveAs->setIcon(QIcon(":/Icons/SaveAs"));

    this->addSeparator();

    m_actionClose = MenuBar::toolBar()->addAction(tr("&Close"));
    m_actionClose->setShortcut(QKeySequence::Close);
    m_actionClose->setIcon(QIcon(":/Icons/Close"));
    m_actionExport = this->addAction(tr("&Export"));
    this->addAction(m_actionClose);


    this->addSeparator();

    m_actionExit = this->addAction(tr("&Exit"));
    m_actionExit->setShortcut(QKeySequence::Quit);
    connect(m_actionExit, SIGNAL(triggered()), qApp, SLOT(quit()));
}
