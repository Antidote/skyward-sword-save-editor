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

bool WiiKeys::LoadKeys()
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
        SetNGPriv(tmp);
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
        SetNGSig(tmp);
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
        SetMacAddr(tmp);
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

void WiiKeys::SaveKeys()
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

bool WiiKeys::IsOpen() const
{
    return m_open;
}

bool WiiKeys::isValid() const
{
    return (m_ngID > 0 && m_ngKeyID > 0 && m_ngPriv != NULL && m_ngSig != NULL && m_macAddr != NULL);
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
