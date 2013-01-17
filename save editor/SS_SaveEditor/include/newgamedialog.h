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

#ifndef NEWGAMEDIALOG_H
#define NEWGAMEDIALOG_H

#include <QDialog>
#include "igamefile.h"
#include "skywardswordfile.h"

class QAbstractButton;

namespace Ui {
class NewGameDialog;
}

class NewGameDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit NewGameDialog(QWidget *parent = 0, SkywardSwordFile::Game game = SkywardSwordFile::Game1);
    ~NewGameDialog();

    void gameFile(SkywardSwordFile* gameFile);

private:
    Ui::NewGameDialog* m_ui;
    SkywardSwordFile::Game        m_game;
};

#endif // NEWGAMEDIALOG_H
