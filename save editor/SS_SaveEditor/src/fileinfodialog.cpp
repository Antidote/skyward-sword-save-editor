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
#include <QDebug>

FileInfoDialog::FileInfoDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::FileInfoDialog),
    m_gameFile(NULL)
{
    m_ui->setupUi(this);
    connect(m_ui->regionBtnGrp, SIGNAL(buttonClicked(int)), this, SLOT(onRegionChanged(int)));
}

FileInfoDialog::~FileInfoDialog()
{
    delete m_ui;
}

void FileInfoDialog::setGameFile(SkywardSwordFile *game)
{
    if (game)
        m_gameFile = game;
}

void FileInfoDialog::showEvent(QShowEvent *se)
{

    if (m_gameFile == NULL)
    {
        QDialog::showEvent(se);
        m_ui->fileInfoGrpBox->hide();
        return;
    }

    QPixmap pixmap = m_gameFile->banner();
    m_ui->bannerImg->setPixmap(pixmap);
    m_ui->titleLbl->setText("Title: " + m_gameFile->bannerTitle());
    m_ui->subtitleLbl->setText("Subtitle: " + m_gameFile->bannerSubtitle());

    this->setWindowIcon(m_gameFile->icon());

    qDebug() << m_gameFile->region();
    switch(m_gameFile->region())
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

    SkywardSwordFile::Game oldGame = m_gameFile->game();
    for (int i = 0; i < 3; i++)
    {
        m_gameFile->setGame((SkywardSwordFile::Game)i);
        if (!m_gameFile->isNew())
            count++;
    }
    m_gameFile->setGame(oldGame);

    m_ui->checkSumLbl->setText(tr("Adventure Checksum: 0x").append(QString("").sprintf("%08X", m_gameFile->checksum())));
    m_ui->adventureCountLbl->setText(tr("Adventure Count: %1").arg(count));
    m_ui->currentAdventureLbl->setText(m_gameFile->isNew() ? tr("New Adventure") : tr("Current Adventure: %1 - %2").arg(m_gameFile->game() + 1).arg(m_gameFile->playerName()));
    QDialog::showEvent(se);
}

static int regionConv[] =
{
    SkywardSwordFile::NTSCURegion,
    SkywardSwordFile::NTSCJRegion,
    SkywardSwordFile::PALRegion
};

#include <QDebug>
void FileInfoDialog::onRegionChanged(int b)
{
    // QButtonGroup does some weird shit to the index
    m_region = regionConv[(abs(b + 2))];
    qDebug() << (abs(b + 2));

    m_ui->titleLbl->setText("Title: " + regionString(m_region, Title));
    m_ui->subtitleLbl->setText("Subtitle: " + regionString(m_region, Subtitle));
}

void FileInfoDialog::accept()
{
    if (!m_gameFile)
    {
        QDialog::accept();
        return;
    }

    m_gameFile->setRegion((SkywardSwordFile::Region)m_region);
    QDialog::accept();
}

QString FileInfoDialog::regionString(int region, StringType type) const
{
    if (m_gameFile && (int)m_gameFile->region() == region)
    {
        if (type == Title)
            return m_gameFile->bannerTitle();
        else
            return m_gameFile->bannerSubtitle();
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
