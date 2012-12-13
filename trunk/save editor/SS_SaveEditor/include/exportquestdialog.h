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

#ifndef EXPORTQUESTDIALOG_H
#define EXPORTQUESTDIALOG_H

#include <QDialog>

namespace Ui {
class ExportQuestDialog;
}

class ExportQuestDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit ExportQuestDialog(QWidget *parent = 0);
    ~ExportQuestDialog();
    
private:
    Ui::ExportQuestDialog *ui;
};

#endif // EXPORTQUESTDIALOG_H
