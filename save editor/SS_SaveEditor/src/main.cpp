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

#include <QApplication>

#include <QFile>
#include <QString>
#include <QTextStream>
#include "mainwindow.h"
#include <QCalendarWidget>
#include <QPushButton>
#include <QDebug>


void loadStyleSheet() {
    /* Let's use QFile and point to a resource... */
#ifdef Q_OS_WIN
    QFile data(":/css/WindowStyleWin32");
#else
    QFile data(":/css/WindowStyleUnix");
#endif
    QString style;
    /* ...to open the file */
    if(data.open(QFile::ReadOnly)) {
                 /* QTextStream... */
                 QTextStream styleIn(&data);
                 /* ...read file to a string. */
                 style = styleIn.readAll();
                 data.close();
                 /* We'll use qApp macro to get the QApplication pointer
                  * and set the style sheet application wide. */
                 qApp->setStyleSheet(style);
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName("WiiKing2");
    a.setApplicationName("WiiKing2 Editor");
    loadStyleSheet();
    MainWindow w;
    w.show();

    return a.exec();
}
