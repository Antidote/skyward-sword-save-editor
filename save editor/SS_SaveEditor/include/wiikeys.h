#ifndef WIIKEYS_H
#define WIIKEYS_H

#include <QByteArray>
#include <QString>

class WiiKeys
{
public:
    bool Open(const QString& filepath);
    bool IsOpen();

    QByteArray GetNGPriv() const;
    quint32    GetNGID() const;
    quint32    GetNGKeyID() const;
    QByteArray GetNGSig() const;
    QByteArray GetMacAddr() const;
    void       SetMacAddr(const QByteArray& mac);

    static WiiKeys*   GetInstance();
private:
    char*   m_ngPriv;
    char*   m_ngSig;
    char*   m_macAddr;
    quint32 m_ngID;
    quint32 m_ngKeyID;
    bool    m_isOpen;
    WiiKeys();
    WiiKeys(WiiKeys&){};

    static WiiKeys* m_instance;
};

#endif // WIIKEYS_H
