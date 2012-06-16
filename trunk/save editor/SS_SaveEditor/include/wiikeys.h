#ifndef WIIKEYS_H
#define WIIKEYS_H

#include <QByteArray>
#include <QString>

class WiiKeys
{
public:
    bool Open(const QString& filepath);
    bool LoadRegistry();
    bool IsOpen();

    QByteArray GetNGPriv() const;
    void       SetNGPriv(const QByteArray& priv);
    quint32    GetNGID() const;
    void       SetNGID(const quint32 id);
    quint32    GetNGKeyID() const;
    void       SetNGKeyID(const quint32 keyId);
    QByteArray GetNGSig() const;
    void       SetNGSig(const QByteArray& sig);
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
    WiiKeys(WiiKeys&){}

    static WiiKeys* m_instance;
};

#endif // WIIKEYS_H
