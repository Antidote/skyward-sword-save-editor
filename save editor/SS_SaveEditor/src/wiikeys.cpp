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
    m_ngID(NULL),
    m_ngKeyID(NULL),
    m_isOpen(false)
{
}

bool WiiKeys::Open(const QString &filepath)
{
    if (filepath.size() <= 0)
        return false;

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
            m_ngPriv = new char[0x1E];
        if (!m_ngSig)
            m_ngSig = new char[0x3C];

        fseek(f, 0x128, SEEK_SET);
        fread(m_ngPriv, 1, 0x1E, f);
        fseek(f, 0x124, SEEK_SET);
        fread(&m_ngID, 1, 0x04, f);
        m_ngID = qFromBigEndian<quint32>(m_ngID);
        fseek(f, 0x208, SEEK_SET);
        fread(&m_ngKeyID, 1,  0x04, f);
        m_ngKeyID = qFromBigEndian<quint32>(m_ngKeyID);
        fseek(f, 0x20C, SEEK_SET);
        fread(m_ngSig, 1, 0x3C, f);
        m_isOpen = true;
        fclose(f);
        return true;
    }

    m_isOpen = false;
    return false;
}

bool WiiKeys::LoadRegistry()
{
    QByteArray tmp;
    bool ok = false;

    QSettings settings("WiiKing2", "WiiKing2 Editor");

    if (!settings.allKeys().count() > 0)
        return false;

    tmp = QByteArray::fromHex(settings.value("NGID").toByteArray());
    if (tmp.size() <= 8 && !tmp.isEmpty())
    {
        qDebug() << tmp.toHex();
        m_ngID = tmp.toHex().toInt(&ok, 16);
        if(ok)
            qDebug() << "Found NGID Successfully";
        else
        {
            m_ngID = 0;
            qDebug() << "NGID Malformed";
        }
    }
    else
    {
        m_ngID = 0;
        qDebug() << "NGID Malformed";
    }

    tmp = QByteArray::fromHex(settings.value("NGKeyID").toByteArray());
    if (tmp.size() <= 8 && !tmp.isEmpty())
    {
        qDebug() << tmp.toHex();
        m_ngKeyID = tmp.toHex().toInt(&ok, 16);
        if(ok)
            qDebug() << "Found NGKeyID Successfully";
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
    if (tmp.size() <= 60 && !tmp.isEmpty())
    {
        SetNGPriv(tmp);
        qDebug() << tmp.toHex();
        qDebug() << "Found NGPriv Successfully";
    }
    else
    {
        if (m_ngPriv)
            delete[] m_ngPriv;
        m_ngPriv = NULL;
        qDebug() << "NGPriv Not found";
    }

    tmp = QByteArray::fromHex(settings.value("NGSig").toByteArray());
    if (tmp.size() <= 120 && !tmp.isEmpty())
    {
        SetNGSig(tmp);
        qDebug() << tmp.toHex();
        qDebug() << "Found NGSig Successfully";
    }
    else
    {
        if (m_ngSig)
            delete[] m_ngSig;
        m_ngSig = NULL;
        qDebug() << tmp.toHex();
        qDebug() << "NGSig Not found";
    }
    tmp = QByteArray::fromHex(settings.value("WiiMAC").toByteArray());
    if (tmp.size() <= 12 && !tmp.isEmpty())
    {
        SetMacAddr(tmp);
        qDebug() << tmp.toHex();
        qDebug() << "Found WiiMac Successfully";
    }
    else
    {
        if (m_macAddr)
            delete[] m_macAddr;
        m_macAddr = NULL;
        qDebug() << "WiiMAC Not found";
    }

    if (m_ngID > 0 && m_ngKeyID > 0 && m_ngPriv > 0 && m_ngSig > 0 && m_macAddr > 0)
        return true;
    return false;
}

QByteArray WiiKeys::GetNGPriv() const
{
    return QByteArray(m_ngPriv, 0x1E);
}

void WiiKeys::SetNGPriv(const QByteArray& priv)
{
    if (m_ngPriv)
        delete[] m_ngPriv;

    m_ngPriv = new char[0x1E];
    memcpy(m_ngPriv, priv.constData(), priv.size());
}

quint32 WiiKeys::GetNGID() const
{
    return m_ngID;
}

void WiiKeys::SetNGID(const quint32 id)
{
    m_ngID = id;
}

quint32 WiiKeys::GetNGKeyID() const
{
    return m_ngKeyID;
}

void WiiKeys::SetNGKeyID(const quint32 keyId)
{
    m_ngKeyID = keyId;
}

QByteArray WiiKeys::GetNGSig() const
{
    return QByteArray(m_ngSig, 0x3C);
}

void WiiKeys::SetNGSig(const QByteArray &sig)
{
    if (m_ngSig)
        delete[] m_ngSig;

    m_ngSig = new char[0x3C];
    memcpy(m_ngSig, sig.constData(), sig.size());
}

QByteArray WiiKeys::GetMacAddr() const
{
    return QByteArray(m_macAddr, 0x06);
}

void WiiKeys::SetMacAddr(const QByteArray &mac)
{
    if (m_macAddr)
        delete[] m_macAddr;

    m_macAddr = new char[0x06];
    memcpy(m_macAddr, mac.constData(), mac.size());
}

WiiKeys* WiiKeys::GetInstance()
{
    if (m_instance == NULL)
        m_instance = new WiiKeys;
    return m_instance;
}

WiiKeys* WiiKeys::m_instance = NULL;
