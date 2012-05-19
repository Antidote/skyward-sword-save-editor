#include "wiikeys.h"

#include <QFile>
#include <QtEndian>
#include <QDebug>

WiiKeys::WiiKeys() :
    m_ngPriv(NULL),
    m_ngSig(NULL),
    m_ngID(NULL),
    m_ngKeyID(NULL),
    m_macAddr(NULL),
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
        qDebug() << buf;
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

QByteArray WiiKeys::GetNGPriv() const
{
    return QByteArray(m_ngPriv, 0x1E);
}

quint32 WiiKeys::GetNGID() const
{
    return m_ngID;
}

quint32 WiiKeys::GetNGKeyID() const
{
    return m_ngKeyID;
}

QByteArray WiiKeys::GetNGSig() const
{
    return QByteArray(m_ngSig, 0x3C);
}

QByteArray WiiKeys::GetMacAddr() const
{
    return QByteArray(m_macAddr, 0x06);
}

void WiiKeys::SetMacAddr(const QByteArray &mac)
{
    m_macAddr = (char*)mac.data();
}

WiiKeys* WiiKeys::GetInstance()
{
    if (m_instance == NULL)
        m_instance = new WiiKeys;
    return m_instance;
}

WiiKeys* WiiKeys::m_instance = NULL;
