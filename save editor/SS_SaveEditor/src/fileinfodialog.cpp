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

#include "fileinfodialog.h"
#include "ui_fileinfodialog.h"

#include "igamefile.h"
#include "skywardswordfile.h"

#include <QFile>
#include <QRadioButton>

FileInfoDialog::FileInfoDialog(QWidget *parent, SkywardSwordFile& game) :
    QDialog(parent),
    m_ui(new Ui::FileInfoDialog)
{
    m_ui->setupUi(this);
    m_gameFile = &game;

    QButtonGroup* group = new QButtonGroup(this);
    group->addButton(m_ui->ntscURadioBtn);
    group->addButton(m_ui->ntscJRadioBtn);
    group->addButton(m_ui->palRadioBtn);

    if (m_gameFile != NULL)
    {
        QPixmap pixmap = m_gameFile->GetBanner();
        m_ui->bannerImg->setPixmap(pixmap);
        m_ui->titleLbl->setText("Title: " + m_gameFile->GetBannerTitle());
        m_ui->subtitleLbl->setText("Subtitle: " + m_gameFile->GetBannerSubtitle());

        this->setWindowIcon(m_gameFile->GetIcon());

        switch(m_gameFile->GetRegion())
        {
            case SkywardSwordFile::NTSCURegion:
                m_ui->ntscURadioBtn->setChecked(true);
                break;
            case SkywardSwordFile::NTSCJRegion:
                m_ui->ntscJRadioBtn->setChecked(true);
                break;
            case SkywardSwordFile::PALRegion:
                m_ui->palRadioBtn->setChecked(true);
                break;
        }

        int count = 0;

        SkywardSwordFile::Game oldGame = m_gameFile->GetGame();
        for (int i = 0; i < 3; i++)
        {
            m_gameFile->SetGame((SkywardSwordFile::Game)i);
            if (!m_gameFile->IsNew())
                count++;
        }
        m_gameFile->SetGame(oldGame);

        m_ui->checkSumLbl->setText(tr("Adventure Checksum: 0x").append(QString("").sprintf("%08X", m_gameFile->GetChecksum())));
        m_ui->adventureCountLbl->setText(tr("Adventure Count: %1").arg(count));
        m_ui->currentAdventureLbl->setText(m_gameFile->IsNew() ? tr("New Adventure") : tr("Current Adventure: %1 - %2").arg(m_gameFile->GetGame() + 1).arg(m_gameFile->GetPlayerName()));
    }
    else
    {
        m_ui->fileInfoGrpBox->hide();
    }


    connect(group, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(onRegionChanged(QAbstractButton*)));
    connect(m_ui->buttonBox, SIGNAL(accepted()), this, SLOT(onAccepted()));
}

FileInfoDialog::~FileInfoDialog()
{
    delete m_ui;
}

void FileInfoDialog::onRegionChanged(QAbstractButton *)
{

    if (m_ui->ntscURadioBtn->isChecked())
        m_region = SkywardSwordFile::NTSCURegion;
    else if (m_ui->ntscJRadioBtn->isChecked())
        m_region = SkywardSwordFile::NTSCJRegion;
    else if (m_ui->palRadioBtn->isChecked())
        m_region = SkywardSwordFile::PALRegion;

    m_ui->titleLbl->setText("Title: " + GetRegionString(m_region, Title));
    m_ui->subtitleLbl->setText("Title: " + GetRegionString(m_region, Subtitle));
}

void FileInfoDialog::onAccepted()
{
    if (!m_gameFile)
        return;

    m_gameFile->SetRegion((SkywardSwordFile::Region)m_region);
}

QString FileInfoDialog::GetRegionString(int region, StringType type) const
{
    if (m_gameFile && (int)m_gameFile->GetRegion() == region)
    {
        if (type == Title)
            return m_gameFile->GetBannerTitle();
        else
            return m_gameFile->GetBannerSubtitle();
    }

    QString file;
    switch(type)
    {
    case Title:
        file = "title"; break;
    case Subtitle:
        file = "subtitle"; break;
    }
    char gameId[5];
    memset(gameId, 0, 5);
    memcpy(gameId, (char*)&region, 4);

    QFile title(QString(":/BannerData/%1/%2.bin").arg(gameId).arg(file));
    if (title.open(QFile::ReadOnly))
    {
        QString titleString = QString::fromUtf16((ushort*)title.readAll().data());
        title.close();
        return titleString;
    }

    return QString("");
}
