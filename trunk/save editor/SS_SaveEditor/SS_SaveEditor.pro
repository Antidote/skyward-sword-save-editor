#-------------------------------------------------
#
# Project created by QtCreator 2012-01-15T01:33:43
#
#-------------------------------------------------

QT += core gui


win32:CONFIG(debug, debug|release){
    system("..\\svn_template\\SubWCRev.exe ..\\..\\ ..\\svn_template\\svnrev_template.h ..\\svn_template\\svnrev.h")
}

unix:CONFIG(debug, debug|release){
    system("../svn_template/makesvnrev.sh")
}

CONFIG(debug, debug|release){
    DEFINES += DEBUG
}
CONFIG(release, release|debug){
    DEFINES -= DEBUG
}

TARGET = SS_SaveEditor
QMAKE_CXXFLAGS = -O0 -O1 -O2 -O3 -Os
TEMPLATE = app
INCLUDEPATH += include \
           ../LibWiiSave/include \
           ../libzelda/include
unix:LIBS += -L../LibWiiSave/lib/Linux -lWiiSave -L../libzelda/lib/Linux -lzelda
win32:LIBS +=  -L../LibWiiSave/lib/Win32 -lWiiSave -L../libzelda/lib/Win32 -lzelda

LIBS +=
QMAKE_LFLAGS += -static

SOURCES += \
    src/main.cpp\
    src/mainwindow.cpp \
    src/newgamedialog.cpp \
    src/aboutdialog.cpp \
    src/fileinfodialog.cpp \
    src/skywardswordfile.cpp \
    src/exportquestdialog.cpp \
    src/wiikeys.cpp \
    src/preferencesdialog.cpp \
    src/checksum.cpp \
    src/common.cpp \
    src/qhexedit2/xbytearray.cpp \
    src/qhexedit2/qhexedit_p.cpp \
    src/qhexedit2/qhexedit.cpp \
    src/qhexedit2/commands.cpp \
    src/newfiledialog.cpp \
    src/gameinfowidget.cpp \
    src/settingsmanager.cpp \
    src/playtimewidget.cpp

HEADERS  += \
    include/mainwindow.h \
    include/igamefile.h \
    include/newgamedialog.h \
    include/aboutdialog.h \
    include/fileinfodialog.h \
    include/skywardswordfile.h \
    include/exportquestdialog.h \
    include/wiikeys.h \
    include/preferencesdialog.h \
    include/checksum.h \
    include/common.h \
    include/qhexedit2/xbytearray.h \
    include/qhexedit2/qhexedit_p.h \
    include/qhexedit2/qhexedit.h \
    include/qhexedit2/commands.h \
    include/newfiledialog.h \
    include/gameinfowidget.h \
    include/settingsmanager.h \
    include/playtimewidget.h

FORMS    += \
    forms/mainwindow.ui \
    forms/newgamedialog.ui \
    forms/aboutdialog.ui \
    forms/fileinfodialog.ui \
    forms/exportquestdialog.ui \
    forms/preferencesdialog.ui \
    forms/newfiledialog.ui \
    forms/gameinfowidget.ui \
    forms/playtimewidget.ui

RESOURCES += \
    resources/resources.qrc

OTHER_FILES += \
    resources/mainicon.rc \
    resources/styleWin32.css \
    resources/styleUnix.css

TRANSLATIONS += \
    resources/languages/ja.ts

win32{
    RC_FILE = resources/mainicon.rc
}
