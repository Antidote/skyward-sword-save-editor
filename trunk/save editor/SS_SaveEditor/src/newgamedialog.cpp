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

NewGameDialog::NewGameDialog(QWidget *parent, SkywardSwordFile::Game game, SkywardSwordFile::Region region) :
    QDialog(parent),
    m_ui(new Ui::NewGameDialog),
    m_game(game)
{
    m_ui->setupUi(this);

    QButtonGroup* group = new QButtonGroup(this);
    group->addButton(m_ui->ntscURadioBtn);
    group->addButton(m_ui->ntscJRadioBtn);
    group->addButton(m_ui->palRadioBtn);

    connect(group, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(onRegionChanged(QAbstractButton*)));

    // A bit hacky but it works.
    switch(region)
    {
        case SkywardSwordFile::NTSCURegion:
            m_ui->ntscURadioBtn->setChecked(true);
            break;
        case SkywardSwordFile::NTSCJRegion:
            m_ui->ntscJRadioBtn->setChecked(true);
            m_ui->nameLineEdit->setText(QString::fromUtf8("\u30ea\u30f3\u30af"));
            break;
        case SkywardSwordFile::PALRegion:
            m_ui->palRadioBtn->setChecked(true);
            break;
    }
}

NewGameDialog::~NewGameDialog()
{
    delete m_ui;
}

void NewGameDialog::onRegionChanged(QAbstractButton *button)
{
    if (button == m_ui->ntscJRadioBtn)
                 m_ui->nameLineEdit->setText(QString::fromUtf8("\u30ea\u30f3\u30af"));
    if (button == m_ui->ntscURadioBtn || button == m_ui->palRadioBtn)
                 m_ui->nameLineEdit->setText("Link");
}

SkywardSwordFile* NewGameDialog::gameFile(SkywardSwordFile* gameFile)
{
    if (gameFile == NULL)
                 gameFile = new SkywardSwordFile();

    if (m_ui->ntscURadioBtn->isChecked())
                 gameFile->SetRegion        (SkywardSwordFile::NTSCURegion);
    else if (m_ui->ntscJRadioBtn->isChecked())
                 gameFile->SetRegion        (SkywardSwordFile::NTSCJRegion);
    else if (m_ui->palRadioBtn->isChecked())
                 gameFile->SetRegion        (SkywardSwordFile::PALRegion);

    gameFile->CreateNewGame(m_game); // Create a new Game with defaults.

    gameFile->SetPlayerName (m_ui->nameLineEdit->text());
    gameFile->SetRupees     (m_ui->rupeeSpinBox->value());
    gameFile->SetTotalHP    (m_ui->hpSpinBox->value());
    gameFile->SetCurrentHP  (m_ui->hpSpinBox->value());
    gameFile->SetUnkHP      (m_ui->hpSpinBox->value());
    gameFile->SetNew        (false);
    gameFile->UpdateChecksum();

    return gameFile;
}
