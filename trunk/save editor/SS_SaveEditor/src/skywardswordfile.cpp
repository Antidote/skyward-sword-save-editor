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

#include "skywardswordfile.h"
#include "common.h"
#include "WiiSave.h"
#include "WiiBanner.h"
#include "WiiFile.h"
#include "Exception.hpp"
#include "wiikeys.h"
#include "checksum.h"
#include <QMessageBox>

#include <QtEndian>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <time.h>

// This constructor allows us to create a new save file.
SkywardSwordFile::SkywardSwordFile(Region region) :
    m_filename(QString()),
    m_saveGame(NULL)
{
    // Gentlemen start your checksum engine!!!
    m_checksumEngine = Checksum();

    CreateEmptyFile(region);
    m_isOpen = true;
    m_bannerImage = QImage();
}

SkywardSwordFile::SkywardSwordFile(const QString& filepath, Game game) :
    m_data(NULL),
    m_filename(filepath),
    m_game(game),
    m_isOpen(false),
    m_isDirty(false),
    m_saveGame(NULL)
{
    m_bannerImage = QImage();
    m_checksumEngine = Checksum();
}

SkywardSwordFile::~SkywardSwordFile()
{
    if (m_data)
    {
        delete[] m_data;
        m_data = NULL;
    }

    if (m_saveGame)
    {
        delete m_saveGame;
        m_saveGame = NULL;
    }
}

bool SkywardSwordFile::Open(Game game, const QString& filepath)
{
    if (m_isOpen)
        Close();

    if (m_game != game)
        m_game = game;

    if (!filepath.isEmpty())
        m_filename = filepath;

    if (m_filename.lastIndexOf(".bin") == m_filename.size() - 4)
    {
       return LoadDataBin(m_filename, game);
    }
    else
    {
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
            m_fileChecksum = m_checksumEngine.GetCRC32((unsigned const char*)m_data, 0, 0xFBE0);
            file.close();
            m_isOpen = true;
            return true;
        }
    }

    return false;
}

bool SkywardSwordFile::Save(const QString& filename)
{
    if (!m_isOpen)
        return false;

    if (!filename.isEmpty())
        m_filename = filename;

    if (m_filename.lastIndexOf(".bin") == m_filename.size() - 4)
    {
//#ifdef DEBUG
        return CreateDataBin();
/*#else
            QMessageBox msg(QMessageBox::Warning, "DISABLED", "Data.bin is an experimental feature and support has been disabled in this version");
            msg.exec();
            return false;
#endif*/
    }

    QString tmpFilename = m_filename;
    tmpFilename = tmpFilename.remove(m_filename.lastIndexOf("."), tmpFilename.length() - tmpFilename.lastIndexOf(".")) + ".tmp";
    FILE* f = fopen(tmpFilename.toAscii(), "wb");
    if (f)
    {
        SetSaveTime();
        for (int i = 0; i < GameCount; ++i)
        {
            Game oldGame = GetGame();
            SetGame((Game)i);
            if (!HasValidChecksum())
                UpdateChecksum(); // ensure the file has the correct Checksum
            SetGame(oldGame);
        }
        fwrite(m_data, 1, 0xFBE0, f);
        fclose(f);
        m_fileChecksum = m_checksumEngine.GetCRC32((const uchar*)m_data, 0, 0xFBE0);

        f = fopen(tmpFilename.toAscii(), "rb");
        if (f)
        {
            char* tmpBuf = new char[0xFBE0];
            fread(tmpBuf, 1, 0xFBE0, f);
            fclose(f);
            quint32 tmpChecksum = m_checksumEngine.GetCRC32((const quint8*)tmpBuf, 0, 0xFBE0);
            if (tmpChecksum == m_fileChecksum)
            {
                QFile file(tmpFilename);
                file.remove(m_filename);
                file.rename(tmpFilename, m_filename);
                file.remove(tmpFilename);
                return true;
            }
            else
                return false;
        }
        return false;
    }
    return false;
}

void SkywardSwordFile::CreateNewGame(SkywardSwordFile::Game game)
{
    if (!m_data)
    {
        // Default to NTSC-U Region
        CreateEmptyFile(NTSCURegion);
    }

    if (m_isOpen == false)
    {
        for (int i = 0; i < 3; i++)
        {
            m_game = (Game)i;
            SetNew(true);
            UpdateChecksum();
        }
        m_isOpen = true;
    }

    m_game = game;
    SetSaveTime();
    SetCurrentArea   ("F000");
    SetCurrentRoom   ("F000");
    SetCurrentMap    ("F000");
    SetPlayerPosition(DEFAULT_POS_X, DEFAULT_POS_Y, DEFAULT_POS_Z);
    SetPlayerRotation(0.0f, 0.0f, 0.0f);
    SetCameraPosition(DEFAULT_POS_X, DEFAULT_POS_Y, DEFAULT_POS_Z);
    SetCameraRotation(0.0f, 0.0f, 0.0f);
}

void SkywardSwordFile::CreateEmptyFile(Region region)
{
    // Need to create a new buffer so we can make our changes.
    m_data = new char[0xFBE0];
    // Zero out the buffer, just to make sure we don't have a 'corrupt' file
    memset(m_data, 0, 0xFBE0);
    // Set the region to the specified one.
    SetRegion(region);
    // The game expects adress 0x001F to be 0x1D so do so.
    m_data[0x001F] = 0x1D;

    // Now each game needs to marked as "New" or the game will detect them
    // Why the game uses a non-zero value for new games is beyond me.
    for (int i = 0; i < 3; i++)
    {
        m_game = (IGameFile::Game)i;
        SetNew(true);
    }
    m_game = IGameFile::Game1;
}

void SkywardSwordFile::ExportGame(const QString &filepath, Game game)
{
    ExportGame(filepath, game, GetRegion());
}

void SkywardSwordFile::ExportGame(const QString& filepath, Game game, Region region)
{
    if (game == GameNone)
        game = Game1;
    char* outBuf = new char[0xFBE0];
    memcpy(outBuf, (m_data + 0x20 + (0x53C0 * game)), 0x53C0);
    FILE* out = fopen(filepath.toAscii(), "wb");
    struct Header
    {
        int magic;
        int version;
        int game;
        char padding[0x14];
    };
    Header header;
    header.magic = 0x5641535A;
    header.version = 0;
    header.game = region;
    memset(&header.padding, 0, 0x14);

    fwrite(&header, 1, sizeof(header), out);
    fwrite(outBuf, 1, 0x53C0, out);
    fclose(out);
}

void SkywardSwordFile::DeleteGame(Game game)
{
    if (!m_data)
        return;

    Game oldGame = m_game;
    m_game = game;
    memset((uchar*)(m_data + GetGameOffset()), 0, 0x53BC);
    SetNew(true);
    SetSaveTime();
    switch(GetRegion())
    {
        default:
        case NTSCURegion:
        case PALRegion:
            SetPlayerName("Link");
            break;
        case NTSCJRegion:
            SetPlayerName(QString::fromUtf8("\u30ea\u30f3\u30af"));
            break;
    }
    UpdateChecksum();
    m_game = oldGame;
}

void SkywardSwordFile::DeleteAllGames()
{
    for (int i = 0; i < 3; i++)
        DeleteGame((Game)i);
}

bool SkywardSwordFile::HasFileOnDiskChanged()
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
        quint32 fileChecksum = m_checksumEngine.GetCRC32((unsigned const char*)data, 0, 0xFBE0);
        file.close();

        if (fileChecksum != m_fileChecksum)
            return true;
    }

    return false;
}

void SkywardSwordFile::Close()
{
    if (!m_data)
        return;

    if (m_saveGame)
        delete m_saveGame;

    m_saveGame = NULL;
    delete[] m_data;
    m_data = NULL;
    m_isOpen = false;
    m_bannerImage = QImage();
}

void SkywardSwordFile::Reload(SkywardSwordFile::Game game)
{
    Close();
    Open(game);
}

bool SkywardSwordFile::IsOpen() const
{
    return m_isOpen;
}

bool SkywardSwordFile::HasValidChecksum()
{
    if (!m_data)
        return false;

    return (*(quint32*)(m_data + GetGameOffset() + 0x53BC) == qFromBigEndian<quint32>(m_checksumEngine.GetCRC32((const unsigned char*)m_data, GetGameOffset(), 0x53BC)));
}

SkywardSwordFile::Game SkywardSwordFile::GetGame() const
{
    return m_game;
}

void SkywardSwordFile::SetGame(Game game)
{
    m_game = game;
}

QString SkywardSwordFile::GetFilename() const
{
    return m_filename;
}

void SkywardSwordFile::SetFilename(const QString &filepath)
{
    m_filename = filepath;
}

SkywardSwordFile::Region SkywardSwordFile::GetRegion() const
{
    if (!m_data)
        return NTSCURegion;
    return (Region)(*(quint32*)(m_data));
}

void SkywardSwordFile::SetRegion(SkywardSwordFile::Region val)
{
    if (!m_data)
        return;
    *(quint32*)(m_data) = val;
}

PlayTime SkywardSwordFile::GetPlayTime() const
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
void SkywardSwordFile::SetPlayTime(PlayTime val)
{
    if (!m_data)
        return;
    quint64 totalSeconds = (val.Hours * 60) * 60;
    totalSeconds += val.Minutes * 60;
    totalSeconds += val.Seconds;
    totalSeconds *= TICKS_PER_SECOND;
    *(quint64*)(m_data + GetGameOffset()) = qToBigEndian<quint64>(totalSeconds);
}

QDateTime SkywardSwordFile::GetSaveTime() const
{
    if (!m_data)
        return QDateTime::currentDateTime();
    QDateTime tmp(QDate(2000, 1, 1));
    tmp = tmp.addSecs(qFromBigEndian<quint64>(*(quint64*)(m_data + GetGameOffset() + 0x0008)) / 60750560);
    return tmp;
}

quint64 GetLocalTimeSinceJan1970()
{
    time_t sysTime, tzDiff, tzDST;
    struct tm * gmTime;

    time(&sysTime);

    // Account for DST where needed
    gmTime = localtime(&sysTime);
    if(gmTime->tm_isdst == 1)
        tzDST = 3600;
    else
        tzDST = 0;

    // Lazy way to get local time in sec
    gmTime	= gmtime(&sysTime);
    tzDiff = sysTime - mktime(gmTime);

    return (quint64)(sysTime + tzDiff + tzDST);
}

void SkywardSwordFile::SetSaveTime()
{
    *(qint64*)(m_data + GetGameOffset() + 0x0008) = qToBigEndian<qint64>((GetLocalTimeSinceJan1970() - SECONDS_TO_2000) * 60749440);
}

Vector3 SkywardSwordFile::GetPlayerPosition() const
{
    if (!m_data)
        return Vector3(0.0f, 0.0f, 0.0f);

    return Vector3(swapFloat(*(float*)(m_data + GetGameOffset() + 0x0010)),
                   swapFloat(*(float*)(m_data + GetGameOffset() + 0x0014)),
                   swapFloat(*(float*)(m_data + GetGameOffset() + 0x0018)));
}

void SkywardSwordFile::SetPlayerPosition(float x, float y, float z)
{
    SetPlayerPosition(Vector3(x, y, z));
}

void SkywardSwordFile::SetPlayerPosition(Vector3 pos)
{
    if (!m_data)
        return;
    *(float*)(m_data + GetGameOffset() + 0x0010) = swapFloat(pos.X);
    *(float*)(m_data + GetGameOffset() + 0x0014) = swapFloat(pos.Y);
    *(float*)(m_data + GetGameOffset() + 0x0018) = swapFloat(pos.Z);
}

Vector3 SkywardSwordFile::GetPlayerRotation() const
{
    if (!m_data)
        return Vector3(0, 0, 0);
    return Vector3(swapFloat(*(float*)(m_data + GetGameOffset() + 0x001C)),
                   swapFloat(*(float*)(m_data + GetGameOffset() + 0x0020)),
                   swapFloat(*(float*)(m_data + GetGameOffset() + 0x0024)));
}

void SkywardSwordFile::SetPlayerRotation(float roll, float pitch, float yaw)
{
    SetPlayerRotation(Vector3(roll, pitch, yaw));
}

void SkywardSwordFile::SetPlayerRotation(Vector3 rotation)
{
    if (!m_data)
        return;
    *(float*)(m_data + GetGameOffset() + 0x001C) = swapFloat(rotation.X);
    *(float*)(m_data + GetGameOffset() + 0x0020) = swapFloat(rotation.Y);
    *(float*)(m_data + GetGameOffset() + 0x0024) = swapFloat(rotation.Z);
}

Vector3 SkywardSwordFile::GetCameraPosition() const
{
    if (!m_data)
        return Vector3(0.0f, 0.0f, 0.0f);
    return Vector3(swapFloat(*(float*)(m_data + GetGameOffset() + 0x0028)),
                   swapFloat(*(float*)(m_data + GetGameOffset() + 0x002C)),
                   swapFloat(*(float*)(m_data + GetGameOffset() + 0x0030)));
}

void SkywardSwordFile::SetCameraPosition(float x, float y, float z)
{
    SetCameraPosition(Vector3(x, y, z));
}

void SkywardSwordFile::SetCameraPosition(Vector3 pos)
{
    if (!m_data)
        return;
    *(float*)(m_data + GetGameOffset() + 0x0028) = swapFloat(pos.X);
    *(float*)(m_data + GetGameOffset() + 0x002C) = swapFloat(pos.Y);
    *(float*)(m_data + GetGameOffset() + 0x0030) = swapFloat(pos.Z);
}

Vector3 SkywardSwordFile::GetCameraRotation() const
{
    if (!m_data)
        return Vector3(0.0f, 0.0f, 0.0f);
    return Vector3(swapFloat(*(float*)(m_data + GetGameOffset() + 0x0034)),
                   swapFloat(*(float*)(m_data + GetGameOffset() + 0x0038)),
                   swapFloat(*(float*)(m_data + GetGameOffset() + 0x003C)));
}

void SkywardSwordFile::SetCameraRotation(float roll, float pitch, float yaw)
{
    SetCameraRotation(Vector3(roll, pitch, yaw));
}

void SkywardSwordFile::SetCameraRotation(Vector3 rotation)
{
    if (!m_data)
        return;

    *(float*)(m_data + GetGameOffset() + 0x0034) = swapFloat(rotation.X);
    *(float*)(m_data + GetGameOffset() + 0x0038) = swapFloat(rotation.Y);
    *(float*)(m_data + GetGameOffset() + 0x003C) = swapFloat(rotation.Z);
}

QString SkywardSwordFile::GetPlayerName() const
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

void SkywardSwordFile::SetPlayerName(const QString &name)
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

bool SkywardSwordFile::IsHeroMode() const
{
    if (!m_data)
        return false;

    return GetFlag(0x08FE, 0x08);
}

void SkywardSwordFile::SetHeroMode(bool val)
{
    if (!m_data)
        return;

    SetFlag(0x08FE, 0x08, val);
}

bool SkywardSwordFile::GetIntroViewed() const
{
    if (!m_data)
        return false;

    return *(char*)(m_data + GetGameOffset() + 0x0941) != 0;
}

void SkywardSwordFile::SetIntroViewed(bool val)
{
    if (!m_data)
        return;

    if (val)
        *(char*)(m_data + GetGameOffset() + 0x0941) = 2;
    else
        *(char*)(m_data + GetGameOffset() + 0x0941) = 0;
}


bool SkywardSwordFile::GetSword(Sword sword) const
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
            return GetFlag(0x09FB, 0x10);
        case MasterSword:
            return GetFlag(0x09E4, 0x04);
        case TrueMasterSword:
            return GetFlag(0x09F3, 0x80);
        default:
            return false;
    }
}

void SkywardSwordFile::SetSword(Sword sword, bool val)
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
        case TrueMasterSword: SetFlag(0x09F3, 0x80, val); break;
        default: return;
    }
}

bool SkywardSwordFile::GetEquipment(WeaponEquipment weapon) const
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

void SkywardSwordFile::SetEquipment(WeaponEquipment weapon, bool val)
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

bool SkywardSwordFile::GetBug(Bug bug) const
{
    if (!m_data)
        return false;

    switch(bug)
    {
        case HornetBug:
            return GetFlag(0x08F6, 0x80);
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

void SkywardSwordFile::SetBug(Bug bug, bool val)
{
    if (!m_data)
        return;
    switch(bug)
    {
        case HornetBug:      SetFlag(0x08F6, 0x80, val); break;
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

quint32 SkywardSwordFile::GetBugQuantity(Bug bug) const
{
    if (!m_data)
        return 0;

    switch(bug)
    {
        case HornetBug:
            return GetQuantity(true,  0x0A4C);
        case ButterflyBug:
            return GetQuantity(false, 0x0A4A);
        case DragonflyBug:
            return GetQuantity(true,  0x0A46);
        case FireflyBug:
            return GetQuantity(false, 0x0A44);
        case RhinoBeetleBug:
            return GetQuantity(false, 0x0A4E);
        case LadybugBug:
            return GetQuantity(true, 0x0A4A);
        case SandCicadaBug:
            return GetQuantity(false, 0x0A48);
        case StagBeetleBug:
            return GetQuantity(true,  0x0A44);
        case GrasshopperBug:
            return GetQuantity(true,  0x0A4E);
        case MantisBug:
            return GetQuantity(false, 0x0A4C);
        case AntBug:
            return GetQuantity(true,  0x0A48);
        case RollerBug:
            return GetQuantity(false, 0x0A46);
        default:
            return 0;
    }
}

void SkywardSwordFile::SetBugQuantity(Bug bug, quint32 val)
{
    if (!m_data)
        return;

    switch(bug)
    {
        case HornetBug:     SetQuantity(true,  0x0A4C, val); break;
        case ButterflyBug:  SetQuantity(false, 0x0A4A, val); break;
        case DragonflyBug:  SetQuantity(true,  0x0A46, val); break;
        case FireflyBug:    SetQuantity(false, 0x0A44, val); break;
        case RhinoBeetleBug:SetQuantity(false, 0x0A4E, val); break;
        case LadybugBug:    SetQuantity(true,  0x0A4A, val); break;
        case SandCicadaBug: SetQuantity(false, 0x0A48, val); break;
        case StagBeetleBug: SetQuantity(true,  0x0A44, val); break;
        case GrasshopperBug:SetQuantity(true,  0x0A4E, val); break;
        case MantisBug:     SetQuantity(false, 0x0A4C, val); break;
        case AntBug:        SetQuantity(true,  0x0A48, val); break;
        case RollerBug:     SetQuantity(false, 0x0A46, val); break;
        default: return;
    }
}

bool SkywardSwordFile::GetMaterial(Material material)
{
    if (!m_data)
        return false;

    switch(material)
    {
        case HornetLarvaeMaterial:
            return GetFlag(0x0934, 0x02);
        case BirdFeatherMaterial:
            return GetFlag(0x0934, 0x04);
        case TumbleWeedMaterial:
            return GetFlag(0x0934, 0x08);
        case LizardTailMaterial:
            return GetFlag(0x0934, 0x10);
        case EldinOreMaterial:
            return GetFlag(0x0934, 0x20);
        case AncientFlowerMaterial:
            return GetFlag(0x0934, 0x40);
        case AmberRelicMaterial:
            return GetFlag(0x0934, 0x80);
        case DuskRelicMaterial:
            return GetFlag(0x0937, 0x01);
        case JellyBlobMaterial:
            return GetFlag(0x0937, 0x02);
        case MonsterClawMaterial:
            return GetFlag(0x0937, 0x04);
        case MonsterHornMaterial:
            return GetFlag(0x0937, 0x08);
        case OrnamentalSkullMaterial:
            return GetFlag(0x0937, 0x10);
        case EvilCrystalMaterial:
            return GetFlag(0x0937, 0x20);
        case BlueBirdFeatherMaterial:
            return GetFlag(0x0937, 0x40);
        case GoldenSkullMaterial:
            return GetFlag(0x0937, 0x80);
        case GoddessPlumeMaterial:
            return GetFlag(0x0936, 0x01);
        default:
            return false;
    }
}

void SkywardSwordFile::SetMaterial(Material material, bool val)
{
    if (!m_data)
        return;

    switch(material)
    {
        case HornetLarvaeMaterial:    SetFlag(0x0934, 0x02, val); break;
        case BirdFeatherMaterial:     SetFlag(0x0934, 0x04, val); break;
        case TumbleWeedMaterial:      SetFlag(0x0934, 0x08, val); break;
        case LizardTailMaterial:      SetFlag(0x0934, 0x10, val); break;
        case EldinOreMaterial:        SetFlag(0x0934, 0x20, val); break;
        case AncientFlowerMaterial:   SetFlag(0x0934, 0x40, val); break;
        case AmberRelicMaterial:      SetFlag(0x0934, 0x80, val); break;
        case DuskRelicMaterial:       SetFlag(0x0937, 0x01, val); break;
        case JellyBlobMaterial:       SetFlag(0x0937, 0x02, val); break;
        case MonsterClawMaterial:     SetFlag(0x0937, 0x04, val); break;
        case MonsterHornMaterial:     SetFlag(0x0937, 0x08, val); break;
        case OrnamentalSkullMaterial: SetFlag(0x0937, 0x10, val); break;
        case EvilCrystalMaterial:     SetFlag(0x0937, 0x20, val); break;
        case BlueBirdFeatherMaterial: SetFlag(0x0937, 0x40, val); break;
        case GoldenSkullMaterial:     SetFlag(0x0937, 0x80, val); break;
        case GoddessPlumeMaterial:    SetFlag(0x0936, 0x01, val); break;
        default: return;
    }
}

quint32 SkywardSwordFile::GetMaterialQuantity(Material material)
{
    if (!m_data)
        return 0;

    switch(material)
    {
        case HornetLarvaeMaterial:
            return GetQuantity(true, 0x0A42);
        case BirdFeatherMaterial:
            return GetQuantity(false,0x0A42);
        case TumbleWeedMaterial:
            return GetQuantity(true, 0x0A40);
        case LizardTailMaterial:
            return GetQuantity(false,0x0A40);
        case EldinOreMaterial:
            return GetQuantity(true, 0x0A3E);
        case AncientFlowerMaterial:
            return GetQuantity(false,0x0A3E);
        case AmberRelicMaterial:
            return GetQuantity(true, 0x0A3C);
        case DuskRelicMaterial:
            return GetQuantity(false,0x0A3C);
        case JellyBlobMaterial:
            return GetQuantity(true, 0x0A3A);
        case MonsterClawMaterial:
            return GetQuantity(false,0x0A3A);
        case MonsterHornMaterial:
            return GetQuantity(true, 0x0A38);
        case OrnamentalSkullMaterial:
            return GetQuantity(false,0x0A38);
        case EvilCrystalMaterial:
            return GetQuantity(true, 0x0A36);
        case BlueBirdFeatherMaterial:
            return GetQuantity(false,0x0A36);
        case GoldenSkullMaterial:
            return GetQuantity(true, 0x0A34);
        case GoddessPlumeMaterial:
            return GetQuantity(false,0x0A34);
        default:
            return 0;
    }
}

void SkywardSwordFile::SetMaterialQuantity(Material material, quint32 val)
{
    if (!m_data)
        return;

    switch(material)
    {
        case HornetLarvaeMaterial:   SetQuantity(true,  0x0A42, val); break;
        case BirdFeatherMaterial:    SetQuantity(false, 0x0A42, val); break;
        case TumbleWeedMaterial:     SetQuantity(true,  0x0A40, val); break;
        case LizardTailMaterial:     SetQuantity(false, 0x0A40, val); break;
        case EldinOreMaterial:       SetQuantity(true,  0x0A3E, val); break;
        case AncientFlowerMaterial:  SetQuantity(false, 0x0A3E, val); break;
        case AmberRelicMaterial:     SetQuantity(true,  0x0A3C, val); break;
        case DuskRelicMaterial:      SetQuantity(false, 0x0A3C, val); break;
        case JellyBlobMaterial:      SetQuantity(true,  0x0A3A, val); break;
        case MonsterClawMaterial:    SetQuantity(false, 0x0A3A, val); break;
        case MonsterHornMaterial:    SetQuantity(true,  0x0A38, val); break;
        case OrnamentalSkullMaterial:SetQuantity(false, 0x0A38, val); break;
        case EvilCrystalMaterial:    SetQuantity(true,  0x0A36, val); break;
        case BlueBirdFeatherMaterial:SetQuantity(false, 0x0A36, val); break;
        case GoldenSkullMaterial:    SetQuantity(true,  0x0A34, val); break;
        case GoddessPlumeMaterial:   SetQuantity(false, 0x0A34, val); break;
        default: return;
    }
}

quint32 SkywardSwordFile::GetGratitudeCrystalAmount()
{
    if (!m_data)
        return 0;
    quint32 ret = (quint32)((qFromBigEndian<quint16>(*(quint16*)(m_data + GetGameOffset() + 0x0A50)) >> 3) & 127);
    return ret;
}

void SkywardSwordFile::SetGratitudeCrystalAmount(quint32 val)
{
    if (!m_data)
        return;
    val = (quint16)(val << 3);
    *(quint16*)(m_data + GetGameOffset() + 0x0A50) = qToBigEndian<quint16>((quint16)val);
}

ushort SkywardSwordFile::GetRupees() const
{
    if (!m_data)
        return 0;

    ushort tmp = *(ushort*)(m_data + GetGameOffset() + 0x0A5E);
    return qFromBigEndian<quint16>(tmp);
}

void SkywardSwordFile::SetRupees(ushort val)
{
    if (!m_data)
        return;
    *(ushort*)(m_data + GetGameOffset() + 0x0A5E) = qToBigEndian<quint16>(val);
}

ushort SkywardSwordFile::GetTotalHP() const
{
    if (!m_data)
        return 0;
    return qToBigEndian<quint16>(*(ushort*)(m_data + GetGameOffset() + 0x5302));
}

void SkywardSwordFile::SetTotalHP(ushort val)
{
    if (!m_data)
        return;

    *(ushort*)(m_data + GetGameOffset() + 0x5302) = qToBigEndian<quint16>(val);
}

ushort SkywardSwordFile::GetUnkHP() const
{
    if (!m_data)
        return 0;

    return qFromBigEndian<quint16>(*(ushort*)(m_data + GetGameOffset() + 0x5304));
}

void SkywardSwordFile::SetUnkHP(ushort val)
{
    if (!m_data)
        return;

    *(ushort*)(m_data + GetGameOffset() + 0x5304) = qToBigEndian<quint16>(val);
}

ushort SkywardSwordFile::GetCurrentHP() const
{
    if (!m_data)
        return 0;

    return qFromBigEndian<quint16>(*(quint16*)(m_data + GetGameOffset() + 0x5306));
}

void SkywardSwordFile::SetCurrentHP(ushort val)
{
    if (!m_data)
        return;
    *(ushort*)(m_data + GetGameOffset() + 0x5306) = qToBigEndian<quint16>(val);
}

uint SkywardSwordFile::GetRoomID() const
{
    return (uint)(*(uchar*)(m_data + GetGameOffset() + 0x5309));
}

void SkywardSwordFile::SetRoomID(uint val)
{
    *(uchar*)(m_data + GetGameOffset() + 0x5309) = (uchar)val;
}

QString SkywardSwordFile::GetCurrentMap() const
{
    return ReadNullTermString(GetGameOffset() + 0x531c);
}

void SkywardSwordFile::SetCurrentMap(const QString& map)
{
    WriteNullTermString(map, GetGameOffset() + 0x531c);
}

QString SkywardSwordFile::GetCurrentArea() const
{
    return ReadNullTermString(GetGameOffset() + 0x533c);
}

void SkywardSwordFile::SetCurrentArea(const QString& map)
{
    WriteNullTermString(map, GetGameOffset() + 0x533c);
}

QString SkywardSwordFile::GetCurrentRoom() const // Not sure about this one
{
    return ReadNullTermString(GetGameOffset() + 0x535c);
}

void SkywardSwordFile::SetCurrentRoom(const QString& map) // Not sure about this one
{
    WriteNullTermString(map, GetGameOffset() + 0x535c);
}

quint8* SkywardSwordFile::GetSkipData() const
{
    if (!m_data)
        return NULL;
    quint8* skip = new quint8[0x80];
    memcpy(skip, (m_data + 0x20 + (0x53BC * 3)), 0x80);

    return skip;
}

void SkywardSwordFile::SetSkipData(const quint8 *data)
{
    memcpy((m_data + 0x20 + (0x53BC * 3)), data, 0x80);
}

uint SkywardSwordFile::GetChecksum() const
{
    if (!m_data)
        return 0;

    return qFromBigEndian<quint32>(*(quint32*)(m_data + GetGameOffset() + 0x53bc));
}

uint SkywardSwordFile::GetGameOffset() const
{
    if (!m_data)
        return 0;

    return (0x20 + (0x53C0 * m_game));
}

void SkywardSwordFile::UpdateChecksum()
{
    if (!m_data)
        return;

    *(uint*)(m_data + GetGameOffset() + 0x53BC) =  qToBigEndian<quint32>(m_checksumEngine.GetCRC32((const unsigned char*)m_data, GetGameOffset(), 0x53BC)); // change it to Big Endian
}

bool SkywardSwordFile::IsNew() const
{
    if (!m_data)
        return true;

    return (*(char*)(m_data + GetGameOffset() + 0x53AD)) != 0;
}

void SkywardSwordFile::SetNew(bool val)
{
    *(char*)(m_data + GetGameOffset() + 0x53AD) = val;
}

bool SkywardSwordFile::IsModified() const
{
    // HACK: Is this really a hack? :/
    quint32 newCrc = ((Checksum*)&m_checksumEngine)->GetCRC32((const quint8*)m_data, 0, 0xFBE0);

    return !(newCrc == m_fileChecksum);
}

QString SkywardSwordFile::ReadNullTermString(int offset) const
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

void SkywardSwordFile::WriteDataFile(const QString &filepath, char* data, quint64 len)
{
    QFileInfo fileInfo("data/" + filepath);
    if (!fileInfo.exists())
    {
        QDir dir;
        if (!dir.exists(fileInfo.path()))
            dir.mkdir(fileInfo.path());

        FILE* f = fopen(fileInfo.filePath().toStdString().c_str(), "wb");
        if (f)
        {
            fwrite(data, 1, len, f);
            fclose(f);
        }
    }
}

void SkywardSwordFile::WriteNullTermString(const QString& val, int offset)
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

bool SkywardSwordFile::GetFlag(quint32 offset, quint32 flag) const
{
    return (*(char*)(m_data + GetGameOffset() + offset) & flag) == flag;
}

void SkywardSwordFile::SetFlag(quint32 offset, quint32 flag, bool val)
{
    if (val)
        *(char*)(m_data + GetGameOffset() + offset) |= flag;
    else
        *(char*)(m_data + GetGameOffset() + offset) &= ~flag;
}

bool SkywardSwordFile::IsValidFile(const QString &filepath, Region* outRegion)
{

    FILE* file = fopen(filepath.toAscii(), "rb");
    if (!file)
        return false;

    Region region;
    fread(&region, 4, 1, file);
    fseek(file, 0, SEEK_END);
    quint32 size = ftell(file);
    fclose(file);
    *outRegion = region;
    return (region == NTSCURegion || region == NTSCJRegion || region == PALRegion) && size == 0xFBE0;
}

quint32 SkywardSwordFile::GetQuantity(bool isRight, int offset) const
{
    if (!m_data)
        return 0;
    switch(isRight)
    {
        case false:
            return (quint32)(qFromBigEndian<quint16>((*(quint16*)(m_data + GetGameOffset() + offset))) >> 7) & 127;
        case true:
            return (quint32)(qFromBigEndian<quint16>(*(quint16*)(m_data + GetGameOffset() + offset))) & 127;
        default:
            return 0;
    }
}

void SkywardSwordFile::SetQuantity(bool isRight, int offset, quint32 val)
{
    if (!m_data)
        return;

    quint16 oldVal = qFromBigEndian<quint16>(*(quint16*)(m_data + GetGameOffset() + offset));
    switch(isRight)
    {
        case false:
        {
            quint16 newVal = (oldVal&127)|(((quint16)val << 7));
            *(quint16*)(m_data + GetGameOffset() + offset) = qToBigEndian<quint16>(newVal);
        }
        break;
        case true:
        {
            oldVal = (oldVal >> 7) & 127;
            quint16 newVal = (val|(oldVal << 7));
            *(quint16*)(m_data + GetGameOffset() + offset) = qToBigEndian<quint16>(newVal);
        }
        break;
    }
}

void SkywardSwordFile::SetData(char *data)
{
    if (m_data)
        delete[] m_data;

    m_data = data;
    m_isOpen = true;
}

bool SkywardSwordFile::LoadDataBin(const QString& filepath, Game game)
{
    if (!filepath.isEmpty())
        m_filename = filepath;

    try
    {
        if (m_saveGame != NULL)
        {
            delete m_saveGame;
            m_saveGame = NULL;
        }

        if (m_data != NULL)
            delete m_data;

        m_saveGame = new WiiSave(m_filename.toStdString());

        char gameId[5];
        int tmp = (int)m_saveGame->banner()->gameID() & 0xFFFFFFFF;
        tmp = qFromBigEndian(tmp);
        memset(gameId, 0, 5);
        memcpy(gameId, (char*)&tmp, 4);

        if (tmp == SkywardSwordFile::NTSCURegion || tmp == SkywardSwordFile::NTSCJRegion || tmp == SkywardSwordFile::PALRegion)
        {
            m_data = (char*)m_saveGame->getFile("/wiiking2.sav")->data();
            UpdateChecksum();
            m_fileChecksum = m_checksumEngine.GetCRC32((const quint8*)m_data, 0, 0xFBE0);
            m_game = game;
            m_isOpen = true;
            return true;
        }
        else
        {
            m_isOpen = false;
            m_game = IGameFile::GameNone;
            return false;
        }
    }
    catch (Exception e)
    {
        QMessageBox msg(QMessageBox::Warning, "Error loading file", e.getMessage().c_str());
        msg.exec();
    }
    catch (std::string what)
    {
        QMessageBox msg(QMessageBox::Warning, "Error loading file", what.c_str());
        msg.exec();
    }

    return false;
}

bool SkywardSwordFile::CreateDataBin()
{
    if (m_saveGame != NULL)
    {
        WiiFile* wiiking2 = m_saveGame->getFile("/wiiking2.sav");
        wiiking2->setData((unsigned char*)m_data);
        m_saveGame->saveToFile(m_filename.toStdString(), (u8*)WiiKeys::GetInstance()->GetMacAddr().data(), WiiKeys::GetInstance()->GetNGID(),(u8*)WiiKeys::GetInstance()->GetNGPriv().data(), (u8*)WiiKeys::GetInstance()->GetNGSig().data(), WiiKeys::GetInstance()->GetNGKeyID());
        m_fileChecksum = m_checksumEngine.GetCRC32((unsigned const char*)m_data, 0, 0xFBE0);
        return true;
    }
    else
    {
        m_saveGame = new WiiSave();
        int region = GetRegion();
        char gameId[5];
        memset(gameId, 0, 5);
        memcpy(gameId, (char*)&region, 4);
        qDebug() << gameId;
        QFile banner(":/BannerData/banner.tpl");
        if (banner.open(QFile::ReadOnly))
        {
            QDataStream dataStream(&banner);
            char* bannerData = new char[192*168*2];
            dataStream.readRawData(bannerData, 192*168*2);
            banner.close();
            WiiBanner* wiiBanner = NULL;

            wiiBanner = new WiiBanner();
            wiiBanner->setBannerImage(new WiiImage(192, 64, (quint8*)bannerData));
            quint64 titleId = 0x00010000;
            quint64 fullId = ((quint64)region << 32)  | qToBigEndian(titleId) >> 32;
            wiiBanner->setGameID(qFromBigEndian(fullId));

            QFile icon(":/BannerData/icon.tpl");
            if (icon.open(QFile::ReadOnly))
            {
                QDataStream dataStream(&icon);
                quint8* iconData = new quint8[48*48*2];
                dataStream.readRawData((char*)iconData, 192*168*2);
                icon.close();
                wiiBanner->addIcon(new WiiImage(48, 48, iconData));
            }
            else
            {
                delete wiiBanner;
                wiiBanner = NULL;
                delete m_saveGame;
                m_saveGame = NULL;
                return false;
            }

            QFile title(QString(":/BannerData/%1/title.bin").arg(gameId));
            if (title.open(QFile::ReadOnly))
            {
                QString titleString = QString::fromUtf16((ushort*)title.readAll().data());
                wiiBanner->setTitle(titleString.toUtf8().data());
                title.close();
            }
            else
            {
                delete wiiBanner;
                wiiBanner = NULL;
                delete m_saveGame;
                m_saveGame = NULL;
                return false;
            }

            title.setFileName(QString(":/BannerData/%1/subtitle.bin").arg(gameId));
            if (title.open(QFile::ReadOnly))
            {
                QString titleString = QString::fromUtf16((ushort*)title.readAll().data());
                wiiBanner->setSubtitle(titleString.toUtf8().data());
                title.close();
            }
            else
            {
                delete wiiBanner;
                wiiBanner = NULL;
                delete m_saveGame;
                m_saveGame = NULL;
                return false;
            }

            wiiBanner->setPermissions(WiiFile::GroupRW | WiiFile::OwnerRW);
            wiiBanner->setAnimationSpeed(0); // no animations
            m_saveGame->setBanner(wiiBanner);
            m_saveGame->addFile("/wiiking2.sav", new WiiFile("wiiking2.sav", WiiFile::GroupRW | WiiFile::OwnerRW, (quint8*)m_data, 0xFBE0));
            m_saveGame->addFile("/skip.dat", new WiiFile("skip.dat", WiiFile::GroupRW | WiiFile::OwnerRW, GetSkipData(), 0x80));
            m_saveGame->saveToFile(m_filename.toStdString(), (u8*)WiiKeys::GetInstance()->GetMacAddr().data(), WiiKeys::GetInstance()->GetNGID(),(u8*)WiiKeys::GetInstance()->GetNGPriv().data(), (u8*)WiiKeys::GetInstance()->GetNGSig().data(), WiiKeys::GetInstance()->GetNGKeyID());
            m_fileChecksum = m_checksumEngine.GetCRC32((unsigned const char*)m_data, 0, 0xFBE0);

            return true;
        }
        else
        {
            delete m_saveGame;
            m_saveGame = NULL;
            return false;
        }
    }
    return false;
}

QString SkywardSwordFile::GetBannerTitle() const
{
    if (m_saveGame != NULL)
    {
        return QString::fromUtf8(m_saveGame->banner()->title().c_str());
    }
    return QString("");
}

const QIcon SkywardSwordFile::GetIcon() const
{
    if (!m_saveGame)
        return QIcon();

    WiiImage* icon = m_saveGame->banner()->getIcon(0);
    if (!icon)
        return QIcon();
    QIcon iconImage(QPixmap::fromImage(ConvertTextureToImage(QByteArray((char*)icon->data(), icon->width()*icon->height()*2), icon->width(), icon->height())));
    return iconImage;
}

const QPixmap SkywardSwordFile::GetBanner() const
{
    if (!m_saveGame)
        return QPixmap();

    WiiImage* banner = m_saveGame->banner()->bannerImage();
    return QPixmap::fromImage(ConvertTextureToImage(QByteArray((char*)banner->data(), banner->width()*banner->height()*2), banner->width(), banner->height()));
}

// To support MSVC I have placed these here, why can't Microsoft follow real ANSI Standards? <.<
const float SkywardSwordFile::DEFAULT_POS_X = -4798.150391;
const float SkywardSwordFile::DEFAULT_POS_Y =  1237.629517;
const float SkywardSwordFile::DEFAULT_POS_Z = -6573.722656;

