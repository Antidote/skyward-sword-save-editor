#ifndef CRC32_H
#define CRC32_H

#include <QByteArray>

#define CRC32_POLYNOMIAL 0x04C11DB7

class CRC32
{
public:
        CRC32();
        quint32 GetCRC32(const unsigned char *sData, quint32 pos, quint32 ulLength );

private:
        quint32 Reflect( quint32 ulReflect, char cChar );
        quint32 m_crcTable[256];
};

#endif
