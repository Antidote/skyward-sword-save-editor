#ifndef WIIKEYS_H
#define WIIKEYS_H

#include <QByteArray>
#include <QString>

class WiiKeys
{
public:
    bool open(const QString& filepath);
    bool loadKeys();
    void saveKeys();
    bool isOpen() const;
    bool isValid() const;

    QByteArray NGPriv() const;
    void       setNGPriv(const QByteArray& priv);
    quint32    NGID() const;
    void       setNGID(const quint32 id);
    quint32    NGKeyID() const;
    void       setNGKeyID(const quint32 keyId);
    QByteArray NGSig() const;
    void       setNGSig(const QByteArray& sig);
    QByteArray macAddr() const;
    void       setMacAddr(const QByteArray& mac);

    static WiiKeys*   instance();


private:
    char*   m_ngPriv;
    char*   m_ngSig;
    char*   m_macAddr;
    quint32 m_ngID;
    quint32 m_ngKeyID;
    bool    m_open;
    WiiKeys();
    WiiKeys(WiiKeys&){}

    static WiiKeys* m_instance;
};

#endif // WIIKEYS_H
