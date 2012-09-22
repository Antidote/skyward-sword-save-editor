// This file is part of WiiKing2 Editor.
//
// WiiKing2 Editor is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Wiiking2 Editor is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with WiiKing2 Editor.  If not, see <http://www.gnu.org/licenses/>

#include "mainwindow.h"
#include "menus/menubar.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
#ifndef Q_OS_WIN
    // Set window icon on non windows OSes, kinda redundant to do it on windows
    this->setWindowIcon(QIcon(":/Icons/Bomb256x256"));
#endif
    this->setMenuBar(MenuBar::instance());
    this->addToolBar(MenuBar::toolBar());
}

MainWindow::~MainWindow()
{
}
