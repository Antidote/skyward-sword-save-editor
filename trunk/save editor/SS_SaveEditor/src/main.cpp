#include <QApplication>

#include <QFile>
#include <QString>
#include <QTextStream>
#include "mainwindow.h"

void loadStyleSheet() {
    /* Let's use QFile and point to a resource... */
    QFile data(":/css/WindowStyle");
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
    loadStyleSheet();
    MainWindow w;
    w.show();
    
    return a.exec();
}
