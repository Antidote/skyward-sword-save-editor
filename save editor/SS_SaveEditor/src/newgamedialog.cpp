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

#include "newgamedialog.h"
#include "ui_newgamedialog.h"
#include "igamefile.h"
#include "skywardswordfile.h"

NewGameDialog::NewGameDialog(QWidget *parent, SkywardSwordFile::Game game) :
    QDialog(parent),
    m_ui(new Ui::NewGameDialog),
    m_game(game)
{
    m_ui->setupUi(this);
}

NewGameDialog::~NewGameDialog()
{
    delete m_ui;
}

void NewGameDialog::gameFile(SkywardSwordFile& gameFile)
{
    SkywardSwordFile* tmpFile = &gameFile;
    if (tmpFile == NULL)
        tmpFile = new SkywardSwordFile(SkywardSwordFile::NTSCURegion);

    tmpFile->CreateNewGame(m_game); // Create a new Game with defaults.

    tmpFile->SetPlayerName (m_ui->nameLineEdit->text());
    tmpFile->SetRupees     (m_ui->rupeeSpinBox->value());
    tmpFile->SetTotalHP    (m_ui->hpSpinBox->value());
    tmpFile->SetCurrentHP  (m_ui->hpSpinBox->value());
    tmpFile->SetUnkHP      (m_ui->hpSpinBox->value());
    tmpFile->SetNew        (false);
    tmpFile->UpdateChecksum();

    gameFile = *tmpFile;
}
