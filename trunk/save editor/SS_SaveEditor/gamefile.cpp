#include "gamefile.h"
#include "CRC32.h"

quint16 swap16(quint16 val)
{
#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
    return ((val >> 8) | (val << 8));
#else
    return val;
#endif
}

quint32 swap32(quint32 val)
{
#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
    return (swap16(val) << 16) | swap16(val >> 16);
#else
    return val;
#endif
}

float swapFloat(float val)
{
#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
    float retVal;
    char* convFloat = (char*) &val;
    char* retFloat = (char*) &retVal;

    retFloat[0] = convFloat[3];
    retFloat[1] = convFloat[2];
    retFloat[2] = convFloat[1];
    retFloat[3] = convFloat[0];

    return retVal;
#else
    return val;
#endif
}

quint64 swap64(quint64 val)
{
#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
    return (quint64)((quint64)swap32(val) << 32 | swap32(val >> 32));
#else
    return val;
#endif
}

GameFile::GameFile(const QString& filepath, Game game) :
    m_data(NULL),
    m_filename(filepath),
    m_game(game),
    m_isOpen(false),
    m_isDirty(false)
{
}

GameFile::~GameFile()
{
    if (!m_data)
    {
        delete[] m_data;
        m_data = NULL;
    }
}

bool GameFile::Open(Game game, const QString& filepath)
{
    if (m_game != game)
        m_game = game;

    if (filepath != NULL)
        m_filename = filepath;

    QFile file(m_filename);

    if (file.open(QIODevice::ReadOnly))
    {
        if (file.size() != 64480)
        {
            file.close();
            return false;
        }

        if (!m_data)
            m_data = new char[0xFBE0];

        file.read(m_data, 0xFBE0);
        file.close();

        return m_isOpen = true;
    }

    return false;
}


bool GameFile::Save(const QString& filename)
{
    if (!m_isOpen)
        return false;

    if (filename != NULL)
        m_filename = filename;

    FILE* f = fopen(m_filename.toAscii(), "rb+");
    if (f)
    {
       if (!HasValidChecksum())
           UpdateChecksum(); // ensure the file has the correct Checksum
        fwrite(m_data, 1, 0xFBE0, f);
        fclose(f);

        m_isDirty = false;

        return true;
    }
    return false;
}

void GameFile::Close()
{
    delete[] m_data;
    m_data = NULL;
    m_isOpen = false;
}

void GameFile::Reload(GameFile::Game game)
{
    Close();
    Open(game);
}

bool GameFile::IsOpen() const
{
    return m_isOpen;
}

bool GameFile::HasValidChecksum()
{
    if (!m_data)
        return false;

    CRC32 crc;
    crc.Initialize();
    return (*(quint32*)(m_data + GetGameOffset() + 0x53BC) == swap32(crc.GetCRC32((const unsigned char*)m_data, GetGameOffset(), 0x53BC)));
}

GameFile::Game GameFile::GetGame() const
{
    return m_game;
}

void GameFile::SetGame(Game game)
{
    m_game = game;
}

QString GameFile::GetFilename() const
{
    return m_filename;
}

float GameFile::GetPlayerX() const
{
    return swapFloat(*(float*)(m_data + GetGameOffset() + 0x0010));
}

void GameFile::SetPlayerX(float val)
{
    *(float*)(m_data + GetGameOffset() + 0x0010) = swapFloat(val);
}

float GameFile::GetPlayerY() const
{
    return swapFloat(*(float*)(m_data + GetGameOffset() + 0x0014));
}

void GameFile::SetPlayerY(float val)
{
    *(float*)(m_data + GetGameOffset() + 0x0014) = swapFloat(val);
}

float GameFile::GetPlayerZ() const
{
    return swapFloat(*(float*)(m_data + GetGameOffset() + 0x0018));
}

void GameFile::SetPlayerZ(float val)
{
    *(float*)(m_data + GetGameOffset() + 0x0018) = swapFloat(val);
}

float GameFile::GetPlayerRoll() const
{
    return swapFloat(*(float*)(m_data + GetGameOffset() + 0x001C));
}

void GameFile::SetPlayerRoll(float val)
{
    *(float*)(m_data + GetGameOffset() + 0x001C) = swapFloat(val);
}

float GameFile::GetPlayerPitch() const
{
    return swapFloat(*(float*)(m_data + GetGameOffset() + 0x0020));
}

void GameFile::SetPlayerPitch(float val)
{
    *(float*)(m_data + GetGameOffset() + 0x0020) = swapFloat(val);
}

float GameFile::GetPlayerYaw() const
{
    return swapFloat(*(float*)(m_data + GetGameOffset() + 0x0024));
}

void GameFile::SetPlayerYaw(float val)
{
    *(float*)(m_data + GetGameOffset() + 0x0024) = swapFloat(val);
}

float GameFile::GetCameraX() const
{
    return swapFloat(*(float*)(m_data + GetGameOffset() + 0x0028));
}

void GameFile::SetCameraX(float val)
{
    *(float*)(m_data + GetGameOffset() + 0x0028) = swapFloat(val);
}

float GameFile::GetCameraY() const
{
    return swapFloat(*(float*)(m_data + GetGameOffset() + 0x002C));
}

void GameFile::SetCameraY(float val)
{
    *(float*)(m_data + GetGameOffset() + 0x002C) = swapFloat(val);
}

float GameFile::GetCameraZ() const
{
    return swapFloat(*(float*)(m_data + GetGameOffset() + 0x0030));
}

void GameFile::SetCameraZ(float val)
{
    *(float*)(m_data + GetGameOffset() + 0x0030) = swapFloat(val);
}

float GameFile::GetCameraRoll() const
{
    return swapFloat(*(float*)(m_data + GetGameOffset() + 0x0034));
}

void GameFile::SetCameraRoll(float val)
{
    *(float*)(m_data + GetGameOffset() + 0x0034) = swapFloat(val);
}

float GameFile::GetCameraPitch() const
{
    return swapFloat(*(float*)(m_data + GetGameOffset() + 0x0038));
}

void GameFile::SetCameraPitch(float val)
{
    *(float*)(m_data + GetGameOffset() + 0x0038) = swapFloat(val);
}

float GameFile::GetCameraYaw() const
{
    return swapFloat(*(float*)(m_data + GetGameOffset() + 0x003C));
}

void GameFile::SetCameraYaw(float val)
{
    *(float*)(m_data + GetGameOffset() + 0x003C) = swapFloat(val);
}


QString GameFile::GetPlayerName() const
{
    if (!m_data)
        return QString("");

    ushort tmpName[8];
    for (int i = 0, j=0; i < 8; ++i, j+= 2)
    {
        tmpName[i] = *(ushort*)(m_data + GetGameOffset() + (0x08D4 + j));
        tmpName[i] = swap16(tmpName[i]);
    }
    tmpName[8] = 0;
    return QString(QString::fromUtf16(tmpName));
}

void GameFile::SetPlayerName(const QString &name)
{
    if (!m_data)
        return;

    for (int i = 0, j = 0; i < 8; ++i, ++j)
    {
        *(ushort*)(m_data + GetGameOffset() + (0x08D4 + j++)) = swap16(name.utf16()[i]);
    }

    m_isDirty = true;
}

ushort GameFile::GetRupees() const
{
    if (!m_data)
        return 0;

    ushort tmp = *(ushort*)(m_data + GetGameOffset() + 0x0A5E);
    return swap16(tmp);
}

void GameFile::SetRupees(ushort val)
{
    if (!m_data)
        return;
    *(ushort*)(m_data + GetGameOffset() + 0x0A5E) = swap16(val);
    m_isDirty = true;
}

ushort GameFile::GetTotalHP() const
{
    if (!m_data)
        return 0;
    return swap16(*(ushort*)(m_data + GetGameOffset() + 0x5302));
}

void GameFile::SetTotalHP(ushort val)
{
    if (!m_data)
        return;

    *(ushort*)(m_data + GetGameOffset() + 0x5302) = swap16(val);
    m_isDirty = true;
}

ushort GameFile::GetUnkHP() const
{
    if (!m_data)
        return 0;

    return swap16(*(ushort*)(m_data + GetGameOffset() + 0x5304));
}

void GameFile::SetUnkHP(ushort val)
{
    if (!m_data)
        return;

    *(ushort*)(m_data + GetGameOffset() + 0x5304) = swap16(val);
    m_isDirty = true;
}

ushort GameFile::GetCurrentHP() const
{
    if (!m_data)
        return 0;

    return swap16(*(ushort*)(m_data + GetGameOffset() + 0x5306));
}

void GameFile::SetCurrentHP(ushort val)
{
    if (!m_data)
        return;
    *(ushort*)(m_data + GetGameOffset() + 0x5306) = swap16(val);
    m_isDirty = true;
}

QString GameFile::GetCurrentMap() const
{
    return ReadNullTermString(GetGameOffset() + 0x531c);
}

void GameFile::SetCurrentMap(const QString& map)
{
    WriteNullTermString(map, GetGameOffset() + 0x531c);
}

QString GameFile::GetCurrentArea() const
{
    return ReadNullTermString(GetGameOffset() + 0x533c);
}

void GameFile::SetCurrentArea(const QString& map)
{
    WriteNullTermString(map, GetGameOffset() + 0x533c);
}

QString GameFile::GetCurrentRoom() const // Not sure about this one
{
    return ReadNullTermString(GetGameOffset() + 0x535c);
}

void GameFile::SetCurrentRoom(const QString& map) // Not sure about this one
{
    WriteNullTermString(map, GetGameOffset() + 0x535c);
}

uint GameFile::GetChecksum() const
{
    if (!m_data)
        return 0;

    return swap32(*(quint32*)(m_data + GetGameOffset() + 0x53bc));
}

uint GameFile::GetGameOffset() const
{
    if (!m_data)
        return 0;

    return (0x20 + (0x53C0 * m_game));
}

void GameFile::UpdateChecksum()
{
    if (!m_data)
        return;

    CRC32 crc;
    crc.Initialize();
    *(uint*)(m_data + GetGameOffset() + 0x53BC) =  swap32(crc.GetCRC32((const unsigned char*)m_data, GetGameOffset(), 0x53BC)); // change it to Big Endian
    m_isDirty = true;
}

bool GameFile::IsModified() const
{
    return m_isDirty;
}

QString GameFile::ReadNullTermString(int offset) const
{
    QString ret("");
    char c = m_data[offset];
    while (c != '\0')
    {
        ret.append(c);
        c = m_data[++offset];
    }

    return ret;
}

void GameFile::WriteNullTermString(const QString& val, int offset)
{
    char c = val.toStdString().c_str()[0];
    int i = 0;
    while (c != '\0')
    {
        m_data[offset++] = c;
        c = val.toStdString().c_str()[++i];
    }
    m_data[offset++] = '\0';
}
