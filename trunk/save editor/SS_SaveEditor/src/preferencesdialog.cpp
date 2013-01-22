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

#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"
#include "wiikeys.h"
#include "settingsmanager.h"
#include <QDir>
#include <QFileInfo>
#include <QApplication>
#include <QFileDialog>
#include <QDebug>

class HexValidator : public QValidator
{
public:
    HexValidator(QObject* parent = 0) :
        QValidator(parent)
    {}
protected:
    QValidator::State validate(QString &input, int &pos) const
    {
        Q_UNUSED(pos)
        input = input.toUpper();
        // match against needed regexp
        QRegExp rx("(?:[0-9a-fA-F]{2})*[0-9a-fA-F]{0,2}");
        if (rx.exactMatch(input)) {
            return QValidator::Acceptable;
        }
        return QValidator::Invalid;
    }
};

PreferencesDialog::PreferencesDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::PreferencesDialog)
{
    m_ui->setupUi(this);

    // Key Settings
    m_ui->ngIDLineEdit->setValidator(new HexValidator(this));
    m_ui->ngKeyIDLineEdit->setValidator(new HexValidator(this));
    m_ui->ngSigPt1LineEdit->setValidator(new HexValidator(this));
    m_ui->ngSigPt2LineEdit->setValidator(new HexValidator(this));
    m_ui->ngPrivLineEdit->setValidator(new HexValidator(this));
    m_ui->macAddrLineEdit->setValidator(new HexValidator(this));
    connect(m_ui->ngSigPt1LineEdit, SIGNAL(textChanged(QString)), this, SLOT(onTextChanged(QString)));
    connect(m_ui->ngSigPt2LineEdit, SIGNAL(textChanged(QString)), this, SLOT(onTextChanged(QString)));
    connect(m_ui->loadKeysBtn, SIGNAL(clicked()), this, SLOT(onLoadKeys()));

    // Region Settings
    SettingsManager* settings = SettingsManager::instance();

    switch(settings->defaultRegion())
    {
        case SettingsManager::NTSCU: m_ui->ntscURB->setChecked(true); break;
        case SettingsManager::NTSCJ: m_ui->ntscJRB->setChecked(true); break;
        case SettingsManager::PAL:   m_ui->palRB  ->setChecked(true); break;
    }

    m_ui->ntscUNameLE->setText(settings->defaultPlayerNameForRegion(SettingsManager::NTSCU));
    m_ui->ntscJNameLE->setText(settings->defaultPlayerNameForRegion(SettingsManager::NTSCJ));
    m_ui->palNameLE  ->setText(settings->defaultPlayerNameForRegion(SettingsManager::PAL  ));
}

PreferencesDialog::~PreferencesDialog()
{
    delete m_ui;
}

void PreferencesDialog::onTextChanged(QString string)
{
    if (((m_ui->ngSigPt1LineEdit->isModified() && m_ui->ngSigPt1LineEdit->hasAcceptableInput())
         || (m_ui->ngSigPt2LineEdit->isModified() && m_ui->ngSigPt2LineEdit->hasAcceptableInput())) && string.length() <= 120)
    {
        if (string.length() > 60)
        {
            m_ui->ngSigPt2LineEdit->setText(string.mid(60));
            m_ui->ngSigPt1LineEdit->setText(string.remove(60, 60));
        }
    }
}

void PreferencesDialog::accept()
{
    if (m_ui->ngIDLineEdit->hasAcceptableInput() && m_ui->ngIDLineEdit->text().length() == 8)
    {
        quint32 id;
        bool ok;
        id = m_ui->ngIDLineEdit->text().toInt(&ok, 16);
        if (ok)
            WiiKeys::instance()->setNGID(id);
    }
    if (m_ui->ngKeyIDLineEdit->hasAcceptableInput() && m_ui->ngKeyIDLineEdit->text().length() == 8)
    {
        quint32 id;
        bool ok;
        id = m_ui->ngKeyIDLineEdit->text().toInt(&ok, 16);
        if (ok)
            WiiKeys::instance()->setNGKeyID(id);
    }
    if (m_ui->ngSigPt1LineEdit->hasAcceptableInput() && m_ui->ngSigPt2LineEdit->hasAcceptableInput() &&
        m_ui->ngSigPt1LineEdit->text().length() == 60 && m_ui->ngSigPt1LineEdit->text().length() == 60)
    {
        QByteArray ngSig = QByteArray::fromHex(m_ui->ngSigPt1LineEdit->text().toAscii());
        ngSig.append(QByteArray::fromHex(m_ui->ngSigPt2LineEdit->text().toAscii()));
        WiiKeys::instance()->setNGSig(ngSig);
    }

    if (m_ui->ngPrivLineEdit->hasAcceptableInput() && m_ui->ngPrivLineEdit->text().length() == 60)
    {
        QByteArray ngPriv = QByteArray::fromHex(m_ui->ngPrivLineEdit->text().toAscii());
        WiiKeys::instance()->setNGPriv(ngPriv);
    }
    if (m_ui->macAddrLineEdit->hasAcceptableInput() && m_ui->macAddrLineEdit->text().length() == 12)
    {
        QByteArray macAddr = QByteArray::fromHex(m_ui->macAddrLineEdit->text().toAscii());
        WiiKeys::instance()->setMacAddr(macAddr);
    }

    SettingsManager* settings = SettingsManager::instance();

    QString regionBtn = m_ui->regionBtnGrp->checkedButton()->objectName();
    settings->setDefaultRegion(regionBtn == "ntscURB" ? SettingsManager::NTSCU :
                              (regionBtn == "ntscJRB" ? SettingsManager::NTSCJ :
                               SettingsManager::PAL));

    if (m_ui->ntscUNameLE->isModified() && !m_ui->ntscUNameLE->text().isEmpty())
        settings->setDefaultPlayerNameForRegion(SettingsManager::NTSCU, m_ui->ntscUNameLE->text());
    if (m_ui->ntscJNameLE->isModified() && !m_ui->ntscJNameLE->text().isEmpty())
        settings->setDefaultPlayerNameForRegion(SettingsManager::NTSCJ, m_ui->ntscJNameLE->text());
    if (m_ui->palNameLE->isModified() && !m_ui->palNameLE->text().isEmpty())
        settings->setDefaultPlayerNameForRegion(SettingsManager::PAL, m_ui->palNameLE->text());

    QDialog::accept();
}

void PreferencesDialog::showEvent(QShowEvent *se)
{
    m_ui->ngIDLineEdit->setText(QString(QByteArray::fromHex(QString::number(WiiKeys::instance()->NGID(), 16).toAscii()).toHex()));
    m_ui->ngKeyIDLineEdit->setText(QString(QByteArray::fromHex(QString::number(WiiKeys::instance()->NGKeyID(), 16).toAscii()).toHex()));
    m_ui->ngSigPt1LineEdit->setText(WiiKeys::instance()->NGSig().remove(30, 30).toHex());
    m_ui->ngSigPt2LineEdit->setText(WiiKeys::instance()->NGSig().mid(30).toHex());
    m_ui->ngPrivLineEdit->setText(WiiKeys::instance()->NGPriv().toHex());
    m_ui->macAddrLineEdit->setText(WiiKeys::instance()->macAddr().toHex());

    QDialog::showEvent(se);
}

void PreferencesDialog::onLoadKeys()
{
    QFileInfo finfo(qApp->applicationDirPath() + "/keys.bin");
    if (!finfo.exists())
    {
        QString fileName = QFileDialog::getOpenFileName(this, "Load Keys", qApp->applicationDirPath(), "BootMii keys.bin (*.bin)");

        if (!fileName.isEmpty())
        {

            if (WiiKeys::instance()->open(fileName, true))
            {
                this->hide();
                this->show();
            }
        }
    }
}
