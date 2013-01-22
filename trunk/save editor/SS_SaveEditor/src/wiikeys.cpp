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

#include "wiikeys.h"

#include <QFile>
#include <QtEndian>
#include <QDebug>
#include <QSettings>
#include <QStringList>

WiiKeys::WiiKeys() :
    m_ngPriv(NULL),
    m_ngSig(NULL),
    m_macAddr(NULL),
    m_ngID(0),
    m_ngKeyID(0),
    m_open(false)
{
}

WiiKeys::~WiiKeys()
{
    saveKeys();
}

bool WiiKeys::open(const QString &filepath, bool clear)
{
    if (filepath.isEmpty())
        return false;

    if (clear)
    {
        if (m_ngPriv)
        {
            delete[] m_ngPriv;
            m_ngPriv = NULL;
        }

        if (m_ngSig)
        {
            delete[] m_ngSig;
            m_ngSig = NULL;
        }

        if (m_ngID != 0)
            m_ngID = 0;
        if (m_ngKeyID != 0)
            m_ngKeyID = 0;
    }

    FILE* f = fopen(filepath.toAscii(), "rb");

    if (f)
    {
        fseek(f, 0, SEEK_END);
        quint32 size = ftell(f);
        fseek(f, 0, SEEK_SET);

        if (size != 0x400)
            return false;

        char buf[0x0C + 1];
        memset(buf, 0, 0x0D);
        fread(buf, 1, 0x0C, f);
        if (strcmp(buf, "BackupMii v1"))
            return false;

        if (!m_ngPriv)
        {
            m_ngPriv = new char[0x1E];
            fseek(f, 0x128, SEEK_SET);
            fread(m_ngPriv, 1, 0x1E, f);
        }
        if (!m_ngSig)
        {
            m_ngSig = new char[0x3C];
            fseek(f, 0x20C, SEEK_SET);
            fread(m_ngSig, 1, 0x3C, f);
        }

        if (m_ngID == 0)
        {
            fseek(f, 0x124, SEEK_SET);
            fread(&m_ngID, 1, 0x04, f);
            m_ngID = qFromBigEndian<quint32>(m_ngID);
        }

        if (m_ngKeyID == 0)
        {
            fseek(f, 0x208, SEEK_SET);
            fread(&m_ngKeyID, 1,  0x04, f);
            m_ngKeyID = qFromBigEndian<quint32>(m_ngKeyID);
        }
        fclose(f);

        m_open = true;
        return true;
    }
    return false;
}

bool WiiKeys::loadKeys()
{
    QByteArray tmp;
    bool ok = false;

    QSettings settings;


    settings.beginGroup("Keys");
    foreach(QString s, settings.allKeys())
        qDebug() << s;
    if (settings.allKeys().count() <= 0)
        return false;

    tmp = QByteArray::fromHex(settings.value("NGID").toByteArray());
    if (tmp.size() == 4 && !tmp.isEmpty())
    {
        m_ngID = tmp.toHex().toInt(&ok, 16);
        if(ok)
            qDebug() << "Found NGID Successfully: " << tmp.toHex();
        else
        {
            m_ngID = 0;
            qDebug() << "NGID Malformed";
        }
    }
    else
    {
        m_ngID = 0;
        qDebug() << "NGID Not found";
    }

    tmp = QByteArray::fromHex(settings.value("NGKeyID").toByteArray());
    if (tmp.size() == 4 && !tmp.isEmpty())
    {
        m_ngKeyID = tmp.toHex().toInt(&ok, 16);
        if(ok)
            qDebug() << "Found NGKeyID Successfully: " << tmp.toHex();
        else
        {
            m_ngKeyID = 0;
            qDebug() << "NGKeyID Malformed";
        }
    }
    else
    {
        m_ngKeyID = 0;
        qDebug() << "NGKeyID Not found";
    }
    tmp = QByteArray::fromHex(settings.value("NGPriv").toByteArray());
    if (tmp.size() == 30 && !tmp.isEmpty())
    {
        setNGPriv(tmp);
        qDebug() << "Found NGPriv Successfully: " << tmp.toHex();
    }
    else
    {
        if (m_ngPriv)
            delete[] m_ngPriv;
        m_ngPriv = NULL;
        qDebug() << "NGPriv Not found";
    }

    tmp = QByteArray::fromHex(settings.value("NGSig").toByteArray());
    if (tmp.size() == 60 && !tmp.isEmpty())
    {
        setNGSig(tmp);
        qDebug() << "Found NGSig Successfully: " << tmp.toHex();
    }
    else
    {
        if (m_ngSig)
            delete[] m_ngSig;
        m_ngSig = NULL;
        qDebug() << "NGSig Not found";
    }
    tmp = QByteArray::fromHex(settings.value("WiiMAC").toByteArray());
    if (tmp.size() == 6 && !tmp.isEmpty())
    {
        setMacAddr(tmp);
        qDebug() << "Found WiiMac Successfully: " << tmp.toHex();
    }
    else
    {
        if (m_macAddr)
            delete[] m_macAddr;
        m_macAddr = NULL;
        qDebug() << "WiiMAC Not found";
    }

    settings.endGroup();
    if (m_ngID > 0 && m_ngKeyID > 0 && m_ngPriv != NULL && m_ngSig != NULL && m_macAddr != NULL)
        return m_open = true;

    return false;
}

void WiiKeys::saveKeys()
{
    QByteArray tmp;
    QSettings settings;
    settings.beginGroup("Keys");
    tmp = QByteArray::fromHex(QString::number(m_ngID, 16).toAscii());
    settings.setValue("NGID", (tmp.size() == 4 ? tmp.toHex() : QByteArray()));
    tmp = QByteArray::fromHex(QString::number(m_ngKeyID, 16).toAscii());
    settings.setValue("NGKeyID", (tmp.size() == 4 ? tmp.toHex() : QByteArray()));
    settings.setValue("NGSig",  QByteArray(m_ngSig, 0x3C).toHex());
    settings.setValue("NGPriv", QByteArray(m_ngPriv, 0x1E).toHex());
    settings.setValue("WiiMAC", QByteArray(m_macAddr, 0x06).toHex());
    settings.endGroup();
}

bool WiiKeys::isOpen() const
{
    return m_open;
}

bool WiiKeys::isValid() const
{
    return (m_ngID > 0 && m_ngKeyID > 0 && m_ngPriv != NULL && m_ngSig != NULL && m_macAddr != NULL);
}

QByteArray WiiKeys::NGPriv() const
{
    return QByteArray(m_ngPriv, 0x1E);
}

void WiiKeys::setNGPriv(const QByteArray& priv)
{
    if (m_ngPriv)
        delete[] m_ngPriv;

    m_ngPriv = new char[0x1E];
    memcpy(m_ngPriv, priv.constData(), priv.size());
}

quint32 WiiKeys::NGID() const
{
    return m_ngID;
}

void WiiKeys::setNGID(const quint32 id)
{
    m_ngID = id;
}

quint32 WiiKeys::NGKeyID() const
{
    return m_ngKeyID;
}

void WiiKeys::setNGKeyID(const quint32 keyId)
{
    m_ngKeyID = keyId;
}

QByteArray WiiKeys::NGSig() const
{
    return QByteArray(m_ngSig, 0x3C);
}

void WiiKeys::setNGSig(const QByteArray &sig)
{
    if (m_ngSig)
        delete[] m_ngSig;

    m_ngSig = new char[0x3C];
    memcpy(m_ngSig, sig.constData(), sig.size());
}

QByteArray WiiKeys::macAddr() const
{
    return QByteArray(m_macAddr, 0x06);
}

void WiiKeys::setMacAddr(const QByteArray &mac)
{
    if (m_macAddr)
        delete[] m_macAddr;

    m_macAddr = new char[0x06];
    memcpy(m_macAddr, mac.constData(), mac.size());
}

WiiKeys* WiiKeys::instance()
{
    if (m_instance == NULL)
        m_instance = new WiiKeys;
    return m_instance;
}

WiiKeys* WiiKeys::m_instance = NULL;
