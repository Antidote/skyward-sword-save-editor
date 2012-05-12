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

#include "gamefile.h"
#include <QtEndian>

#include <QDateTime>
#include <QDebug>


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

GameFile::GameFile(const QString& filepath, Game game) :
    m_data(NULL),
    m_filename(filepath),
    m_game(game),
    m_isOpen(false)
{
    if (m_filename == NULL)
        m_data = new char[0xFBE0];
    memset(m_data, 0, 0xFBE0);

    *(char*)(m_data + 0x01F) = 0x1D;
    m_crcEngine = new CRC32;
}

GameFile::~GameFile()
{
    if (m_data)
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
        if (file.size() != 0xFBE0)
        {
            file.close();
            return false;
        }

        if (m_data)
        {
            delete[] m_data;
            m_data = NULL;
        }

        m_data = new char[0xFBE0];


        file.read((char*)m_data, 0xFBE0);
        m_fileChecksum = m_crcEngine->GetCRC32((unsigned const char*)m_data, 0, 0xFBE0);
        file.close();
        m_isOpen = true;
        return true;
    }

    return false;
}

bool GameFile::Save(const QString& filename)
{
    if (!m_isOpen)
        return false;

    if (filename != NULL)
        m_filename = filename;

    FILE* f = fopen(m_filename.toAscii(), "wb");
    if (f)
    {
       if (!HasValidChecksum())
           UpdateChecksum(); // ensure the file has the correct Checksum
        fwrite(m_data, 1, 0xFBE0, f);
        fclose(f);

        m_fileChecksum = m_crcEngine->GetCRC32((const uchar*)m_data, 0, 0xFBE0);
        return true;
    }
    return false;
}

void GameFile::CreateNewGame(GameFile::Game game)
{
    if (m_isOpen == false)
    {
        for (int i = 0; i < 3; i++)
        {
            this->m_game = (Game)i;
            this->SetNew(true);
            this->UpdateChecksum();
        }
        this->m_isOpen = true;
    }

    this->m_game = game;
    this->SetCurrentArea   ("F000");
    this->SetCurrentRoom   ("F000");
    this->SetCurrentMap    ("F000");
    this->SetPlayerPosition(DEFAULT_POS_X, DEFAULT_POS_Y, DEFAULT_POS_Z);
    this->SetPlayerRotation(0.0f, 0.0f, 0.0f);
    this->SetCameraPosition(DEFAULT_POS_X, DEFAULT_POS_Y, DEFAULT_POS_Z);
    this->SetCameraRotation(0.0f, 0.0f, 0.0f);
}

void GameFile::DeleteGame(Game game)
{
    if (!m_data)
        return;

    Game oldGame = m_game;
    m_game = game;
    memset((uchar*)(m_data + GetGameOffset()), 0, 0x53BC);
    SetNew(true);
    SetPlayerName("Link");
    UpdateChecksum();
    m_game = oldGame;
}

void GameFile::DeleteAllGames()
{
    for (int i = 0; i < 3; i++)
        DeleteGame((Game)i);
}

bool GameFile::HasFileOnDiskChanged()
{
    if (m_filename.size() <= 0)
        return false; // Currently working in memory only

    QFile file(m_filename);

    if (file.open(QIODevice::ReadOnly))
    {
        // I'm going to go ahead and keep this for now. (Prevents you from accidentally fucking up your save files)
        if (file.size() != 0xFBE0)
        {
            file.close();
            return false;
        }
        char* data = new char[0xFBE0];

        file.read((char*)data, 0xFBE0);
        quint32 fileChecksum = m_crcEngine->GetCRC32((unsigned const char*)data, 0, 0xFBE0);
        file.close();

        if (fileChecksum != m_fileChecksum)
            return true;
    }

    return false;
}

void GameFile::Close()
{
    if (!m_data)
        return;

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

    return (*(quint32*)(m_data + GetGameOffset() + 0x53BC) == qFromBigEndian<quint32>(m_crcEngine->GetCRC32((const unsigned char*)m_data, GetGameOffset(), 0x53BC)));
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

void GameFile::SetFilename(const QString &filepath)
{
    m_filename = filepath;
}

GameFile::Region GameFile::GetRegion() const
{
    return (Region)(*(quint32*)(m_data));
}

void GameFile::SetRegion(GameFile::Region val)
{
    *(quint32*)(m_data) = val;
}

PlayTime GameFile::GetPlayTime() const
{
    if (!m_data)
        return PlayTime();
    PlayTime playTime;
    quint64 tmp = qFromBigEndian<quint64>(*(quint64*)(m_data + GetGameOffset()));
    playTime.Hours = ((tmp / TICKS_PER_SECOND) / 60) / 60;
    playTime.Minutes =  ((tmp / TICKS_PER_SECOND) / 60) % 60;
    playTime.Seconds = ((tmp / TICKS_PER_SECOND) % 60);
    playTime.RawTicks = tmp;
    return playTime;
}

// Sets the current playtime
void GameFile::SetPlayTime(PlayTime val)
{
    if (!m_data)
        return;
    quint64 totalSeconds = (val.Hours * 60) * 60;
    totalSeconds += val.Minutes * 60;
    totalSeconds += val.Seconds;
    totalSeconds *= TICKS_PER_SECOND;
    *(quint64*)(m_data + GetGameOffset()) = qToBigEndian<quint64>(totalSeconds);
}

QDateTime GameFile::GetSaveTime() const
{
    if (!m_data)
        return QDateTime::currentDateTime();
    QDateTime tmp(QDate(2000, 1, 1));
    tmp = tmp.addSecs(qFromBigEndian<quint64>(*(quint64*)(m_data + GetGameOffset() + 0x0008)) / TICKS_PER_SECOND);
    return tmp;
}

// TODO: Abandoned for now (Need to figure out how to do this :/)
void GameFile::SetSaveTime(QDateTime val)
{
    Q_UNUSED(val);
/*
    quint64 time = (quint64)val.toMSecsSinceEpoch();
    qDebug() << "Time " << ((quint64)((time / 1000) + SECONDS_TO_2000) * TICKS_PER_SECOND);
    qDebug() << "\n" << val.toString();
    *(qint64*)(m_data + GetGameOffset() + 0x0008) = qToBigEndian<qint64>((qint64)((time / 1000) * (TICKS_PER_SECOND)));
*/
}

Vector3 GameFile::GetPlayerPosition() const
{
    if (!m_data)
        return Vector3(0.0f, 0.0f, 0.0f);

    return Vector3(swapFloat(*(float*)(m_data + GetGameOffset() + 0x0010)),
                   swapFloat(*(float*)(m_data + GetGameOffset() + 0x0014)),
                   swapFloat(*(float*)(m_data + GetGameOffset() + 0x0018)));
}

void GameFile::SetPlayerPosition(float x, float y, float z)
{
    SetPlayerPosition(Vector3(x, y, z));
}

void GameFile::SetPlayerPosition(Vector3 pos)
{
    if (!m_data)
        return;
    *(float*)(m_data + GetGameOffset() + 0x0010) = swapFloat(pos.X);
    *(float*)(m_data + GetGameOffset() + 0x0014) = swapFloat(pos.Y);
    *(float*)(m_data + GetGameOffset() + 0x0018) = swapFloat(pos.Z);
}

Vector3 GameFile::GetPlayerRotation() const
{
    if (!m_data)
        return Vector3(0, 0, 0);
    return Vector3(swapFloat(*(float*)(m_data + GetGameOffset() + 0x001C)),
                   swapFloat(*(float*)(m_data + GetGameOffset() + 0x0020)),
                   swapFloat(*(float*)(m_data + GetGameOffset() + 0x0024)));
}

void GameFile::SetPlayerRotation(float roll, float pitch, float yaw)
{
    SetPlayerRotation(Vector3(roll, pitch, yaw));
}

void GameFile::SetPlayerRotation(Vector3 rotation)
{
    if (!m_data)
        return;
    *(float*)(m_data + GetGameOffset() + 0x001C) = swapFloat(rotation.X);
    *(float*)(m_data + GetGameOffset() + 0x0020) = swapFloat(rotation.Y);
    *(float*)(m_data + GetGameOffset() + 0x0024) = swapFloat(rotation.Z);
}

Vector3 GameFile::GetCameraPosition() const
{
    if (!m_data)
        return Vector3(0.0f, 0.0f, 0.0f);
    return Vector3(swapFloat(*(float*)(m_data + GetGameOffset() + 0x0028)),
                   swapFloat(*(float*)(m_data + GetGameOffset() + 0x002C)),
                   swapFloat(*(float*)(m_data + GetGameOffset() + 0x0030)));
}

void GameFile::SetCameraPosition(float x, float y, float z)
{
    SetCameraPosition(Vector3(x, y, z));
}

void GameFile::SetCameraPosition(Vector3 pos)
{
    if (!m_data)
        return;
    *(float*)(m_data + GetGameOffset() + 0x0028) = swapFloat(pos.X);
    *(float*)(m_data + GetGameOffset() + 0x002C) = swapFloat(pos.Y);
    *(float*)(m_data + GetGameOffset() + 0x0030) = swapFloat(pos.Z);
}

Vector3 GameFile::GetCameraRotation() const
{
    if (!m_data)
        return Vector3(0.0f, 0.0f, 0.0f);
    return Vector3(swapFloat(*(float*)(m_data + GetGameOffset() + 0x0034)),
                   swapFloat(*(float*)(m_data + GetGameOffset() + 0x0038)),
                   swapFloat(*(float*)(m_data + GetGameOffset() + 0x003C)));
}

void GameFile::SetCameraRotation(float roll, float pitch, float yaw)
{
    SetCameraRotation(Vector3(roll, pitch, yaw));
}

void GameFile::SetCameraRotation(Vector3 rotation)
{
    if (!m_data)
        return;

    *(float*)(m_data + GetGameOffset() + 0x0034) = swapFloat(rotation.X);
    *(float*)(m_data + GetGameOffset() + 0x0038) = swapFloat(rotation.Y);
    *(float*)(m_data + GetGameOffset() + 0x003C) = swapFloat(rotation.Z);
}

QString GameFile::GetPlayerName() const
{
    if (!m_data)
        return QString("");

    ushort tmpName[8];
    for (int i = 0, j=0; i < 8; ++i, j+= 2)
    {
        tmpName[i] = *(ushort*)(m_data + GetGameOffset() + (0x08D4 + j));
        tmpName[i] = qFromBigEndian<quint16>(tmpName[i]);
    }

    return QString(QString::fromUtf16(tmpName));
}

void GameFile::SetPlayerName(const QString &name)
{
    if (!m_data)
        return;

    for (int i = 0, j = 0; i < 8; ++i, ++j)
    {
        if (i > name.length())
        {
            *(ushort*)(m_data + GetGameOffset() + (0x08D4 + j++)) = 0;
            continue;
        }
        *(ushort*)(m_data + GetGameOffset() + (0x08D4 + j++)) = qToBigEndian<quint16>(name.utf16()[i]);
    }

}

bool GameFile::IsHeroMode() const
{
    if (!m_data)
        return false;

    return GetFlag(0x08FE, 0x08);
}

void GameFile::SetHeroMode(bool val)
{
    if (!m_data)
        return;

    SetFlag(0x08FE, 0x08, val);
}

bool GameFile::GetIntroViewed() const
{
    if (!m_data)
        return false;

    return *(char*)(m_data + GetGameOffset() + 0x0941) != 0;
}

void GameFile::SetIntroViewed(bool val)
{
    if (!m_data)
        return;

    if (val)
        *(char*)(m_data + GetGameOffset() + 0x0941) = 2;
    else
        *(char*)(m_data + GetGameOffset() + 0x0941) = 0;
}


bool GameFile::GetSword(Sword sword) const
{
    if (!m_data)
        return false;

    switch(sword)
    {
        case PracticeSword:
            return GetFlag(0x09F2, 0x01);
        case GoddessSword:
            return GetFlag(0x09E4, 0x01);
        case LongSword:
            return GetFlag(0x09E4, 0x02);
        case WhiteSword:
            return GetFlag(0x09F3, 0x10);
        case MasterSword:
            return GetFlag(0x09E4, 0x04);
        case TrueMasterSword:
            return GetFlag(0x09FB, 0x80);
        default:
            return false;
    }
}

void GameFile::SetSword(Sword sword, bool val)
{
    if (!m_data)
        return;

    switch (sword)
    {
        case PracticeSword:   SetFlag(0x09F2, 0x01, val); break;
        case GoddessSword:    SetFlag(0x09E4, 0x01, val); break;
        case LongSword:       SetFlag(0x09E4, 0x02, val); break;
        case WhiteSword:      SetFlag(0x09FB, 0x10, val); break;
        case MasterSword:     SetFlag(0x09E4, 0x04, val); break;
        case TrueMasterSword: SetFlag(0x09FB, 0x80, val); break;
        default: return;
    }
}

bool GameFile::GetEquipment(WeaponEquipment weapon) const
{
    if (!m_data)
      return false;

    switch(weapon)
    {
        case SlingshotWeapon:
            return GetFlag(0x09E6, 0x10);
        case ScattershotWeapon:
            return GetFlag(0x09EC, 0x80);
        case BugnetWeapon:
            return GetFlag(0x09E8, 0x01);
        case BigBugnetWeapon:
            return GetFlag(0x09F2, 0x02);
        case BeetleWeapon:
            return GetFlag(0x09E6, 0x20);
        case HookBeetleWeapon:
            return GetFlag(0x09EB, 0x02);
        case QuickBeetleWeapon:
            return GetFlag(0x09EB, 0x04);
        case ToughBeetleWeapon:
            return GetFlag(0x09EB, 0x08);
        case BombWeapon:
            return GetFlag(0x09ED, 0x04);
        case GustBellowsWeapon:
            return GetFlag(0x09E6, 0x02);
        case WhipWeapon:
            return GetFlag(0x09F3, 0x10);
        case ClawshotWeapon:
            return GetFlag(0x09E4, 0x20);
        case BowWeapon:
            return GetFlag(0x09E4,0x10);
        case IronBowWeapon:
            return GetFlag(0x09ED, 0x01);
        case SacredBowWeapon:
            return GetFlag(0x09ED, 0x02);
        case HarpEquipment:
            return GetFlag(0x09F4, 0x02);
        default:
            return false;
    }
}

void GameFile::SetEquipment(WeaponEquipment weapon, bool val)
{
    if (!m_data)
        return;

    switch(weapon)
    {
        case SlingshotWeapon:   SetFlag(0x09E6, 0x10, val); break;
        case ScattershotWeapon: SetFlag(0x09EC, 0x80, val); break;
        case BugnetWeapon:      SetFlag(0x09E8, 0x01, val); break;
        case BigBugnetWeapon:   SetFlag(0x09F2, 0x02, val); break;
        case BeetleWeapon:      SetFlag(0x09E6, 0x20, val); break;
        case HookBeetleWeapon:  SetFlag(0x09EB, 0x02, val); break;
        case QuickBeetleWeapon: SetFlag(0x09EB, 0x04, val); break;
        case ToughBeetleWeapon: SetFlag(0x09EB, 0x08, val); break;
        case BombWeapon:        SetFlag(0x09ED, 0x04, val); break;
        case GustBellowsWeapon: SetFlag(0x09E6, 0x02, val); break;
        case WhipWeapon:        SetFlag(0x09F3, 0x10, val); break;
        case ClawshotWeapon:    SetFlag(0x09E4, 0x20, val); break;
        case BowWeapon:         SetFlag(0x09E4, 0x10, val); break;
        case IronBowWeapon:     SetFlag(0x09ED, 0x01, val); break;
        case SacredBowWeapon:   SetFlag(0x09ED, 0x02, val); break;
        case HarpEquipment:     SetFlag(0x09F4, 0x02, val); break;
        default: return;
    }
}

bool GameFile::GetBug(Bug bug) const
{
    if (!m_data)
        return false;

    switch(bug)
    {
        case HornetBug:
            return GetFlag(0x08F6, 0x08);
        case ButterflyBug:
            return GetFlag(0x09F2, 0x80);
        case DragonflyBug:
            return GetFlag(0x09F5, 0x04);
        case FireflyBug:
            return GetFlag(0x09F5, 0x20);
        case RhinoBeetleBug:
            return GetFlag(0x09F2, 0x08);
        case LadybugBug:
            return GetFlag(0x09F2, 0x40);
        case SandCicadaBug:
            return GetFlag(0x09F5, 0x02);
        case StagBeetleBug:
            return GetFlag(0x09F5, 0x10);
        case GrasshopperBug:
            return GetFlag(0x09F2, 0x04);
        case MantisBug:
            return GetFlag(0x09F2, 0x20);
        case AntBug:
            return GetFlag(0x09F5, 0x01);
        case RollerBug:
            return GetFlag(0x09F5, 0x08);
        default:
            return false;
    }
}

void GameFile::SetBug(Bug bug, bool val)
{
    if (!m_data)
        return;
    switch(bug)
    {
        case HornetBug:      SetFlag(0x08F6, 0x08, val); break;
        case ButterflyBug:   SetFlag(0x09F2, 0x80, val); break;
        case DragonflyBug:   SetFlag(0x09F5, 0x04, val); break;
        case FireflyBug:     SetFlag(0x09F5, 0x20, val); break;
        case RhinoBeetleBug: SetFlag(0x09F2, 0x08, val); break;
        case LadybugBug:     SetFlag(0x09F2, 0x40, val); break;
        case SandCicadaBug:  SetFlag(0x09F5, 0x02, val); break;
        case StagBeetleBug:  SetFlag(0x09F5, 0x10, val); break;
        case GrasshopperBug: SetFlag(0x09F2, 0x04, val); break;
        case MantisBug:      SetFlag(0x09F2, 0x20, val); break;
        case AntBug:         SetFlag(0x09F5, 0x01, val); break;
        case RollerBug:      SetFlag(0x09F5, 0x08, val); break;
        default: return;
    }
}

ushort GameFile::GetRupees() const
{
    if (!m_data)
        return 0;

    ushort tmp = *(ushort*)(m_data + GetGameOffset() + 0x0A5E);
    return qFromBigEndian<quint16>(tmp);
}

void GameFile::SetRupees(ushort val)
{
    if (!m_data)
        return;
    *(ushort*)(m_data + GetGameOffset() + 0x0A5E) = qToBigEndian<quint16>(val);
}

ushort GameFile::GetTotalHP() const
{
    if (!m_data)
        return 0;
    return qToBigEndian<quint16>(*(ushort*)(m_data + GetGameOffset() + 0x5302));
}

void GameFile::SetTotalHP(ushort val)
{
    if (!m_data)
        return;

    *(ushort*)(m_data + GetGameOffset() + 0x5302) = qToBigEndian<quint16>(val);
}

ushort GameFile::GetUnkHP() const
{
    if (!m_data)
        return 0;

    return qFromBigEndian<quint16>(*(ushort*)(m_data + GetGameOffset() + 0x5304));
}

void GameFile::SetUnkHP(ushort val)
{
    if (!m_data)
        return;

    *(ushort*)(m_data + GetGameOffset() + 0x5304) = qToBigEndian<quint16>(val);
}

ushort GameFile::GetCurrentHP() const
{
    if (!m_data)
        return 0;

    return qFromBigEndian<quint16>(*(quint16*)(m_data + GetGameOffset() + 0x5306));
}

void GameFile::SetCurrentHP(ushort val)
{
    if (!m_data)
        return;
    *(ushort*)(m_data + GetGameOffset() + 0x5306) = qToBigEndian<quint16>(val);
}

uint GameFile::GetRoomID() const
{
    return (uint)(*(uchar*)(m_data + GetGameOffset() + 0x5309));
}

void GameFile::SetRoomID(uint val)
{
    *(uchar*)(m_data + GetGameOffset() + 0x5309) = (uchar)val;
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

    return qFromBigEndian<quint32>(*(quint32*)(m_data + GetGameOffset() + 0x53bc));
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

    *(uint*)(m_data + GetGameOffset() + 0x53BC) =  qToBigEndian<quint32>(m_crcEngine->GetCRC32((const unsigned char*)m_data, GetGameOffset(), 0x53BC)); // change it to Big Endian
}

bool GameFile::IsNew() const
{
    return (*(char*)(m_data + GetGameOffset() + 0x53AD)) != 0;
}

void GameFile::SetNew(bool val)
{
    *(char*)(m_data + GetGameOffset() + 0x53AD) = val;
}

bool GameFile::IsModified() const
{
    quint32 newCrc = m_crcEngine->GetCRC32((const quint8*)m_data, 0, 0xFBE0);
    return !(newCrc == m_fileChecksum);
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
    if (!m_data)
        return;

    char c = val.toStdString().c_str()[0];
    int i = 0;
    while (c != '\0')
    {
        m_data[offset++] = c;
        c = val.toStdString().c_str()[++i];
    }
    m_data[offset++] = '\0';
}

bool GameFile::GetFlag(quint32 offset, quint32 flag) const
{
    return (*(char*)(m_data + GetGameOffset() + offset) & flag) == flag;
}

void GameFile::SetFlag(quint32 offset, quint32 flag, bool val)
{
    if (val)
        *(char*)(m_data + GetGameOffset() + offset) |= flag;
    else
        *(char*)(m_data + GetGameOffset() + offset) &= ~flag;
}

bool GameFile::IsValidFile(const QString &filepath, Region* outRegion)
{

    FILE* file = fopen(filepath.toAscii(), "rb");
    if (!file)
        return false;

    Region region;
    fread(&region, 4, 1, file);
    fclose(file);
    *outRegion = region;
    return (region == NTSCURegion || region == NTSCJRegion || region == PALRegion);
}
