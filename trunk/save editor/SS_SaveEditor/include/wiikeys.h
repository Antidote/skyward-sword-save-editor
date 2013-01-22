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

#ifndef WIIKEYS_H
#define WIIKEYS_H

#include <QByteArray>
#include <QString>

class WiiKeys
{
public:
    ~WiiKeys();
    bool open(const QString& filepath, bool clear = false);
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
