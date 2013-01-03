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

#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include "../svn_template/svnrev.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::AboutDialog)
{
    m_ui->setupUi(this);
    connect(m_ui->closeBtn, SIGNAL(clicked()), this, SLOT(close()));

#ifdef DEBUG
    QString verstr(SVN_REV_STR);
    m_ui->abtText->setText(m_ui->abtText->text().replace("$SVNREVSTR$", (!verstr.isEmpty() ? QString("WiiKing2 Editor Revision: <a href=\"http://code.google.com/p/skyward-sword-save-editor/source/detail?r=%1\">r%1</a>").arg(verstr) : QString(""))));
#else
    m_ui->abtText->setText(m_ui->abtText->text().replace("$SVNREVSTR$",""));
#endif
}

AboutDialog::~AboutDialog()
{
    delete m_ui;
}
