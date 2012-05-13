#-------------------------------------------------
#
# Project created by QtCreator 2012-01-15T01:33:43
#
#-------------------------------------------------

QT       += core gui

TARGET = SS_SaveEditor
TEMPLATE = app
INCLUDEPATH += include

SOURCES += \
    src/main.cpp\
    src/mainwindow.cpp \
    src/CRC32.cpp \
    src/newgamedialog.cpp \
    src/aboutdialog.cpp \
    src/fileinfodialog.cpp \
    src/skywardswordfile.cpp

HEADERS  += \
    include/mainwindow.h \
    include/igamefile.h \
    include/CRC32.h \
    include/newgamedialog.h \
    include/aboutdialog.h \
    include/fileinfodialog.h \
    include/skywardswordfile.h

FORMS    += \
    forms/mainwindow.ui \
    forms/newgamedialog.ui \
    forms/aboutdialog.ui \
    forms/fileinfodialog.ui

DEFINES += DEBUG

RESOURCES += \
    resources/resources.qrc

OTHER_FILES += \
    resources/bugs/Woodland Rhino Bettle.png \
    resources/bugs/Volcanic Ladybug.png \
    resources/bugs/Starry Firefly.png \
    resources/bugs/Sky Stag Beetle.png \
    resources/bugs/Sand Cicada.png \
    resources/bugs/Loft Mantis.png \
    resources/bugs/Lanayru Ant.png \
    resources/bugs/Gerudo Dragonfly.png \
    resources/bugs/Faron Grasshopper.png \
    resources/bugs/Eldin Roller.png \
    resources/bugs/Deku Hornet.png \
    resources/bugs/Blessed Butterfly.png \
    resources/equipment/Bags/Seed Satchel Small.png \
    resources/equipment/Bags/Seed Satchel Medium.png \
    resources/equipment/Bags/Seed Satchel Large.png \
    resources/equipment/Bags/Bomb Bag Small.png \
    resources/equipment/Bags/Bomb Bag Medium.png \
    resources/equipment/Bags/Bomb Bag Large.png \
    resources/equipment/Quivers/Quiver Small.png \
    resources/equipment/Quivers/Quiver Medium.png \
    resources/equipment/Quivers/Quiver Large.png \
    resources/equipment/Shields/Wooden Shield.png \
    resources/equipment/Shields/Sacred Shield.png \
    resources/equipment/Shields/Reinforced Shield.png \
    resources/equipment/Shields/Iron Shield.png \
    resources/equipment/Shields/Hylian Shield.png \
    resources/equipment/Shields/Goddess Shield.png \
    resources/equipment/Shields/Fortified Shield.png \
    resources/equipment/Shields/Divine Shield.png \
    resources/equipment/Shields/Braced Shield.png \
    resources/equipment/Shields/Banded Shield.png \
    resources/equipment/Swords/True Master Sword.png \
    resources/equipment/Swords/Practice Sword.png \
    resources/equipment/Swords/Master Sword.png \
    resources/equipment/Swords/Goddess White Sword.png \
    resources/equipment/Swords/Goddess Sword Icon.png \
    resources/equipment/Swords/Goddess Long Sword.png \
    resources/equipment/Wallets/Tycoo Wallet.png \
    resources/equipment/Wallets/Small Wallet.png \
    resources/equipment/Wallets/Mediu Wallet.png \
    resources/equipment/Wallets/Giant Wallet.png \
    resources/equipment/Wallets/Big Wallet.png \
    resources/mainicon.rc

TRANSLATIONS += \
    resources/languages/ja.ts
	
win32{
    RC_FILE = resources/mainicon.rc
}
