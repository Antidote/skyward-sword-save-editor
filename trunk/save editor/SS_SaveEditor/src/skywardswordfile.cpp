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
#include "settingsmanager.h"
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

    createEmptyFile(region);
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
    open(game, filepath);
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

bool SkywardSwordFile::open(Game game, const QString& filepath)
{
    if (m_isOpen)
        close();

    if (m_game != game)
        m_game = game;

    if (!filepath.isEmpty())
        m_filename = filepath;

    if (m_filename.lastIndexOf(".bin") == m_filename.size() - 4)
    {
       return loadDataBin(m_filename, game);
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
            file.close();
            m_isOpen = true;
            return true;
        }
    }

    return false;
}

bool SkywardSwordFile::save(const QString& filename)
{
    if (!m_isOpen)
        return false;

    if (!filename.isEmpty())
        m_filename = filename;

    if (m_filename.lastIndexOf(".bin") == m_filename.size() - 4)
    {
//#ifdef DEBUG
        return saveDataBin();
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
        for (int i = 0; i < GameCount; ++i)
        {
            Game oldGame = game();
            setGame((Game)i);
            if (!hasValidChecksum())
                updateChecksum(); // ensure the file has the correct Checksum
            setGame(oldGame);
        }
        fwrite(m_data, 1, 0xFBE0, f);
        fclose(f);

        f = fopen(tmpFilename.toAscii(), "rb");
        if (f)
        {
            char* tmpBuf = new char[0xFBE0];
            fread(tmpBuf, 1, 0xFBE0, f);
            fclose(f);
            QFile file(tmpFilename);
            if (file.exists() && file.size() == 0xFBE0)
            {

                file.remove(m_filename);
                file.rename(tmpFilename, m_filename);
                file.remove(tmpFilename);
                m_isDirty = false;
                return true;
            }
            else
                return false;
        }
        return false;
    }
    return false;
}

int regionConv[]=
{
    SkywardSwordFile::NTSCURegion,
    SkywardSwordFile::NTSCJRegion,
    SkywardSwordFile::PALRegion
};

void SkywardSwordFile::createNewGame(SkywardSwordFile::Game game)
{
    if (m_isOpen == false)
    {
        int region = SettingsManager::instance()->defaultRegion();
        createEmptyFile((Region)regionConv[region]);
    }

    setGame(game);
    memset(m_data + gameOffset(), 0, 0x53C0);
    setNew(false);
    m_game = game;
    setSaveTime(QDateTime::currentDateTime());
    setCurrentArea   ("F000");
    setCurrentRoom   ("F000");
    setCurrentMap    ("F000");
    setPlayerPosition(DEFAULT_POS_X, DEFAULT_POS_Y, DEFAULT_POS_Z);
    setPlayerRotation(0.0f, 0.0f, 0.0f);
    setCameraPosition(DEFAULT_POS_X, DEFAULT_POS_Y, DEFAULT_POS_Z);
    setCameraRotation(0.0f, 0.0f, 0.0f);
    emit modified();
}

void SkywardSwordFile::createEmptyFile(Region region)
{
    // Need to create a new buffer so we can make our changes.
    m_data = new char[0xFBE0];
    // Zero out the buffer, just to make sure we don't have a 'corrupt' file
    memset(m_data, 0, 0xFBE0);
    // Set the region to the specified one.
    setRegion(region);
    // The game expects adress 0x001F to be 0x1D so do so.
    m_data[0x001F] = 0x1D;

    // Now each game needs to marked as "New" or the game will detect them
    // Why the game uses a non-zero value for new games is beyond me.
    for (int i = 0; i < 3; i++)
    {
        m_game = (IGameFile::Game)i;
        updateChecksum();
        setNew(true);
    }
    m_game = IGameFile::Game1;
    m_isDirty = true;
    m_isOpen = true;
    emit modified();
}

void SkywardSwordFile::exportGame(const QString &filepath, Game game)
{
    exportGame(filepath, game, region());
}

void SkywardSwordFile::exportGame(const QString& filepath, Game game, Region region)
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

void SkywardSwordFile::deleteGame(Game game)
{
    if (!m_data)
        return;

    Game oldGame = m_game;
    m_game = game;
    memset((uchar*)(m_data + gameOffset()), 0, 0x53BC);
    setNew(true);
    updateChecksum();
    m_game = oldGame;
    m_isDirty = true;
    emit modified();
}

void SkywardSwordFile::deleteAllGames()
{
    for (int i = 0; i < 3; i++)
        deleteGame((Game)i);
}

void SkywardSwordFile::close()
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
    m_isDirty = false;
}

bool SkywardSwordFile::reload(SkywardSwordFile::Game game)
{
    close();
    m_isDirty = false;
    return open(game);
}

bool SkywardSwordFile::isOpen() const
{
    return m_isOpen;
}

bool SkywardSwordFile::hasValidChecksum()
{
    if (!m_data)
        return false;

    return (*(quint32*)(m_data + gameOffset() + 0x53BC) == qFromBigEndian<quint32>(m_checksumEngine.CRC32((const unsigned char*)m_data, gameOffset(), 0x53BC)));
}

SkywardSwordFile::Game SkywardSwordFile::game() const
{
    return m_game;
}

void SkywardSwordFile::setGame(Game game)
{
    m_game = game;
}

QString SkywardSwordFile::filename() const
{
    return m_filename;
}

void SkywardSwordFile::setFilename(const QString &filepath)
{
    m_filename = filepath;
}

SkywardSwordFile::Region SkywardSwordFile::region() const
{
    if (!m_data)
        return NTSCURegion;
    return (Region)(*(quint32*)(m_data));
}

void SkywardSwordFile::setRegion(SkywardSwordFile::Region val)
{
    if (!m_data)
        return;

    if (m_saveGame)
    {
        // Set the strings appropriately
        char gameId[5];
        memset(gameId, 0, 5);
        memcpy(gameId, (char*)&val, 4);
        QFile title(QString(":/BannerData/%1/title.bin").arg(gameId));
        if (title.open(QFile::ReadOnly))
        {
            QString titleString = QString::fromUtf16((ushort*)title.readAll().data());
            m_saveGame->banner()->setTitle(titleString.toUtf8().data());
            title.close();
        }

        title.setFileName(QString(":/BannerData/%1/subtitle.bin").arg(gameId));
        if (title.open(QFile::ReadOnly))
        {
            QString titleString = QString::fromUtf16((ushort*)title.readAll().data());
            m_saveGame->banner()->setSubtitle(titleString.toUtf8().data());
            title.close();
        }
    }

    *(quint32*)(m_data) = val;
    m_isDirty = true;
    emit modified();
}

PlayTime SkywardSwordFile::playTime() const
{
    if (!m_data)
        return PlayTime();
    PlayTime playTime;
    quint64 tmp = qFromBigEndian<quint64>(*(quint64*)(m_data + gameOffset()));
    playTime.Days    = (((tmp / TICKS_PER_SECOND) / 60) / 60) / 24;
    playTime.Hours   = (((tmp / TICKS_PER_SECOND) / 60) / 60) % 24;
    playTime.Minutes = (( tmp / TICKS_PER_SECOND) / 60) % 60;
    playTime.Seconds = (( tmp / TICKS_PER_SECOND) % 60);
    return playTime;
}

// Sets the current playtime
void SkywardSwordFile::setPlayTime(PlayTime val)
{
    if (!m_data)
        return;
    quint64 totalSeconds = 0;
    totalSeconds += ((val.Days    * 60) * 60) * 24;
    totalSeconds += ( val.Hours   * 60) * 60;
    totalSeconds += ( val.Minutes * 60);
    totalSeconds +=   val.Seconds;
    *(quint64*)(m_data + gameOffset()) = qToBigEndian<quint64>(TICKS_PER_SECOND * totalSeconds);
    m_isDirty = true;

    this->updateChecksum();
    emit modified();
}

QDateTime SkywardSwordFile::saveTime() const
{
    if (!m_data)
        return QDateTime::currentDateTime();

    return fromWiiTime(qFromBigEndian(*(quint64*)(m_data + gameOffset() + 0x0008)));
}

void SkywardSwordFile::setSaveTime(const QDateTime& time)
{
    *(qint64*)(m_data + gameOffset() + 0x0008) = qToBigEndian<qint64>(toWiiTime(time));
    m_isDirty = true;
    this->updateChecksum();
    emit modified();
}

Vector3 SkywardSwordFile::playerPosition() const
{
    if (!m_data)
        return Vector3(0.0f, 0.0f, 0.0f);

    return Vector3(swapFloat(*(float*)(m_data + gameOffset() + 0x0010)),
                   swapFloat(*(float*)(m_data + gameOffset() + 0x0014)),
                   swapFloat(*(float*)(m_data + gameOffset() + 0x0018)));
}

void SkywardSwordFile::setPlayerPosition(float x, float y, float z)
{
    setPlayerPosition(Vector3(x, y, z));
}

void SkywardSwordFile::setPlayerPosition(Vector3 pos)
{
    if (!m_data)
        return;
    *(float*)(m_data + gameOffset() + 0x0010) = swapFloat(pos.X);
    *(float*)(m_data + gameOffset() + 0x0014) = swapFloat(pos.Y);
    *(float*)(m_data + gameOffset() + 0x0018) = swapFloat(pos.Z);
    m_isDirty = true;
    emit modified();
}

Vector3 SkywardSwordFile::playerRotation() const
{
    if (!m_data)
        return Vector3(0, 0, 0);
    return Vector3(swapFloat(*(float*)(m_data + gameOffset() + 0x001C)),
                   swapFloat(*(float*)(m_data + gameOffset() + 0x0020)),
                   swapFloat(*(float*)(m_data + gameOffset() + 0x0024)));
}

void SkywardSwordFile::setPlayerRotation(float roll, float pitch, float yaw)
{
    setPlayerRotation(Vector3(roll, pitch, yaw));
}

void SkywardSwordFile::setPlayerRotation(Vector3 rotation)
{
    if (!m_data)
        return;
    *(float*)(m_data + gameOffset() + 0x001C) = swapFloat(rotation.X);
    *(float*)(m_data + gameOffset() + 0x0020) = swapFloat(rotation.Y);
    *(float*)(m_data + gameOffset() + 0x0024) = swapFloat(rotation.Z);
    m_isDirty = true;
    updateChecksum();
    emit modified();
}

Vector3 SkywardSwordFile::cameraPosition() const
{
    if (!m_data)
        return Vector3(0.0f, 0.0f, 0.0f);
    return Vector3(swapFloat(*(float*)(m_data + gameOffset() + 0x0028)),
                   swapFloat(*(float*)(m_data + gameOffset() + 0x002C)),
                   swapFloat(*(float*)(m_data + gameOffset() + 0x0030)));
}

void SkywardSwordFile::setCameraPosition(float x, float y, float z)
{
    setCameraPosition(Vector3(x, y, z));
}

void SkywardSwordFile::setCameraPosition(Vector3 pos)
{
    if (!m_data)
        return;
    *(float*)(m_data + gameOffset() + 0x0028) = swapFloat(pos.X);
    *(float*)(m_data + gameOffset() + 0x002C) = swapFloat(pos.Y);
    *(float*)(m_data + gameOffset() + 0x0030) = swapFloat(pos.Z);
    updateChecksum();
    emit modified();
}

Vector3 SkywardSwordFile::cameraRotation() const
{
    if (!m_data)
        return Vector3(0.0f, 0.0f, 0.0f);
    return Vector3(swapFloat(*(float*)(m_data + gameOffset() + 0x0034)),
                   swapFloat(*(float*)(m_data + gameOffset() + 0x0038)),
                   swapFloat(*(float*)(m_data + gameOffset() + 0x003C)));
}

void SkywardSwordFile::setCameraRotation(float roll, float pitch, float yaw)
{
    setCameraRotation(Vector3(roll, pitch, yaw));
}

void SkywardSwordFile::setCameraRotation(Vector3 rotation)
{
    if (!m_data)
        return;

    *(float*)(m_data + gameOffset() + 0x0034) = swapFloat(rotation.X);
    *(float*)(m_data + gameOffset() + 0x0038) = swapFloat(rotation.Y);
    *(float*)(m_data + gameOffset() + 0x003C) = swapFloat(rotation.Z);
    m_isDirty = true;
    emit modified();
}

QString SkywardSwordFile::playerName() const
{
    if (!m_data)
        return QString("");

    ushort tmpName[8];
    for (int i = 0, j=0; i < 8; ++i, j+= 2)
    {
        tmpName[i] = *(ushort*)(m_data + gameOffset() + (0x08D4 + j));
        tmpName[i] = qFromBigEndian<quint16>(tmpName[i]);
    }

    return QString(QString::fromUtf16(tmpName));
}

void SkywardSwordFile::setPlayerName(const QString &name)
{
    if (!m_data)
        return;

    for (int i = 0, j = 0; i < 8; ++i, ++j)
    {
        if (i > name.length())
        {
            *(ushort*)(m_data + gameOffset() + (0x08D4 + j++)) = 0;
            continue;
        }
        *(ushort*)(m_data + gameOffset() + (0x08D4 + j++)) = qToBigEndian<quint16>(name.utf16()[i]);
    }
    m_isDirty = true;
    this->updateChecksum();
    emit modified();
}

bool SkywardSwordFile::isHeroMode() const
{
    if (!m_data)
        return false;

    return flag(0x08FE, 0x08);
}

void SkywardSwordFile::setHeroMode(bool val)
{
    if (!m_data)
        return;

    setFlag(0x08FE, 0x08, val);
    m_isDirty = true;
    this->updateChecksum();
    emit modified();
}

bool SkywardSwordFile::introViewed() const
{
    if (!m_data)
        return false;

    return *(char*)(m_data + gameOffset() + 0x0941) != 0;
}

void SkywardSwordFile::setIntroViewed(bool val)
{
    if (!m_data)
        return;

    if (val)
        *(char*)(m_data + gameOffset() + 0x0941) = 2;
    else
        *(char*)(m_data + gameOffset() + 0x0941) = 0;

    m_isDirty = true;
    this->updateChecksum();
    emit modified();
}

void SkywardSwordFile::practiceSwordChanged(bool val)
{
    this->setSword(PracticeSword, val);
}

void SkywardSwordFile::goddessSwordChanged(bool val)
{
    this->setSword(GoddessSword, val);
}

void SkywardSwordFile::goddessLongSwordChanged(bool val)
{
    this->setSword(LongSword, val);
}

void SkywardSwordFile::goddessWhiteSwordChanged(bool val)
{
    this->setSword(WhiteSword, val);
}

void SkywardSwordFile::masterSwordChanged(bool val)
{
    this->setSword(MasterSword, val);
}

void SkywardSwordFile::trueMasterSwordChanged(bool val)
{
    this->setSword(TrueMasterSword, val);
}

bool SkywardSwordFile::sword(Sword sword) const
{
    if (!m_data)
        return false;

    switch(sword)
    {
        case PracticeSword:
            return flag(0x09F2, 0x01);
        case GoddessSword:
            return flag(0x09E4, 0x01);
        case LongSword:
            return flag(0x09E4, 0x02);
        case WhiteSword:
            return flag(0x09FB, 0x10);
        case MasterSword:
            return flag(0x09E4, 0x04);
        case TrueMasterSword:
            return flag(0x09F3, 0x80);
        default:
            return false;
    }
}

void SkywardSwordFile::setSword(Sword sword, bool val)
{
    if (!m_data)
        return;

    switch (sword)
    {
        case PracticeSword:   setFlag(0x09F2, 0x01, val); break;
        case GoddessSword:    setFlag(0x09E4, 0x01, val); break;
        case LongSword:       setFlag(0x09E4, 0x02, val); break;
        case WhiteSword:      setFlag(0x09FB, 0x10, val); break;
        case MasterSword:     setFlag(0x09E4, 0x04, val); break;
        case TrueMasterSword: setFlag(0x09F3, 0x80, val); break;
        default: return;
    }

    m_isDirty = true;
    this->updateChecksum();
    emit modified();
}

bool SkywardSwordFile::equipment(WeaponEquipment weapon) const
{
    if (!m_data)
      return false;

    switch(weapon)
    {
        case SlingshotWeapon:
            return flag(0x09E6, 0x10);
        case ScattershotWeapon:
            return flag(0x09EC, 0x80);
        case BugnetWeapon:
            return flag(0x09E8, 0x01);
        case BigBugnetWeapon:
            return flag(0x09F2, 0x02);
        case BeetleWeapon:
            return flag(0x09E6, 0x20);
        case HookBeetleWeapon:
            return flag(0x09EB, 0x02);
        case QuickBeetleWeapon:
            return flag(0x09EB, 0x04);
        case ToughBeetleWeapon:
            return flag(0x09EB, 0x08);
        case BombWeapon:
            return flag(0x09ED, 0x04);
        case GustBellowsWeapon:
            return flag(0x09E6, 0x02);
        case WhipWeapon:
            return flag(0x09F3, 0x10);
        case ClawshotWeapon:
            return flag(0x09E4, 0x20);
        case BowWeapon:
            return flag(0x09E4, 0x10);
        case DiggingMittsEquipment:
            return flag(0x09E6, 0x40);
        case WaterDragonScaleEquipment:
            return flag(0x09E9, 0x20);
        case MoleMittsEquipment:
            return flag(0x09EC, 0x02);
        case IronBowWeapon:
            return flag(0x09ED, 0x01);
        case SacredBowWeapon:
            return flag(0x09ED, 0x02);
        case FireShieldEaringsEquipment:
            return flag(0x09F3, 0x20);
        case SailClothEquipment:
            return flag(0x09F4, 0x01);
        case HarpEquipment:
            return flag(0x09F4, 0x02);
        default:
            return false;
    }
}

void SkywardSwordFile::setEquipment(WeaponEquipment weapon, bool val)
{
    if (!m_data)
        return;

    switch(weapon)
    {
        case SlingshotWeapon:           setFlag(0x09E6, 0x10, val); break;
        case ScattershotWeapon:         setFlag(0x09EC, 0x80, val); break;
        case BugnetWeapon:              setFlag(0x09E8, 0x01, val); break;
        case BigBugnetWeapon:           setFlag(0x09F2, 0x02, val); break;
        case BeetleWeapon:              setFlag(0x09E6, 0x20, val); break;
        case HookBeetleWeapon:          setFlag(0x09EB, 0x02, val); break;
        case QuickBeetleWeapon:         setFlag(0x09EB, 0x04, val); break;
        case ToughBeetleWeapon:         setFlag(0x09EB, 0x08, val); break;
        case BombWeapon:                setFlag(0x09ED, 0x04, val); break;
        case GustBellowsWeapon:         setFlag(0x09E6, 0x02, val); break;
        case WhipWeapon:                setFlag(0x09F3, 0x10, val); break;
        case ClawshotWeapon:            setFlag(0x09E4, 0x20, val); break;
        case BowWeapon:                 setFlag(0x09E4, 0x10, val); break;
        case DiggingMittsEquipment:     setFlag(0x09E6, 0x40, val); break;
        case WaterDragonScaleEquipment: setFlag(0x09E9, 0x20, val); break;
        case MoleMittsEquipment:        setFlag(0x09EC, 0x02, val); break;
        case IronBowWeapon:             setFlag(0x09ED, 0x01, val); break;
        case SacredBowWeapon:           setFlag(0x09ED, 0x02, val); break;
        case FireShieldEaringsEquipment:setFlag(0x09F3, 0x20, val); break;
        case SailClothEquipment:        setFlag(0x09F4, 0x01, val); break;
        case HarpEquipment:             setFlag(0x09F4, 0x02, val); break;
        default: return;
    }
    m_isDirty = true;
    this->updateChecksum();
    emit modified();
}

quint32 SkywardSwordFile::ammo(Ammo type)
{
    quint32 val = qFromBigEndian(*(quint32*)(m_data + gameOffset() + 0x0A60));
    quint32 ret = 0;

    switch(type)
    {
        case ArrowAmmo: ret = (val >> 0)  & 127; break;
        case BombAmmo:  ret = (val >> 7)  & 127; break;
        case SeedAmmo:  ret = (val >> 23) & 127; break;
    }

    return ret;
}

void SkywardSwordFile::setAmmo(Ammo type, quint32 val)
{
    quint32 tmp = qFromBigEndian(*(quint32*)(m_data + gameOffset() + 0x0A60));
    quint32 arrows = (tmp >> 0)  & 127;
    quint32 bombs  = (tmp >> 7)  & 127;
    quint32 seeds  = (tmp >> 23) & 127;

    switch(type)
    {
        case ArrowAmmo: arrows = val & 127; break;
        case BombAmmo:  bombs  = val & 127; break;
        case SeedAmmo:  seeds  = val & 127; break;
    }

   *(quint32*)(m_data + gameOffset() + 0x0A60) = qToBigEndian(arrows | (bombs << 7) | (seeds << 23));
    m_isDirty = true;
    this->updateChecksum();
    emit modified();
}

bool SkywardSwordFile::bug(Bug bug) const
{
    if (!m_data)
        return false;

    switch(bug)
    {
        case HornetBug:
            return flag(0x08F6, 0x80);
        case ButterflyBug:
            return flag(0x09F2, 0x80);
        case DragonflyBug:
            return flag(0x09F5, 0x04);
        case FireflyBug:
            return flag(0x09F5, 0x20);
        case RhinoBeetleBug:
            return flag(0x09F2, 0x08);
        case LadybugBug:
            return flag(0x09F2, 0x40);
        case SandCicadaBug:
            return flag(0x09F5, 0x02);
        case StagBeetleBug:
            return flag(0x09F5, 0x10);
        case GrasshopperBug:
            return flag(0x09F2, 0x04);
        case MantisBug:
            return flag(0x09F2, 0x20);
        case AntBug:
            return flag(0x09F5, 0x01);
        case RollerBug:
            return flag(0x09F5, 0x08);
        default:
            return false;
    }
}

void SkywardSwordFile::setBug(Bug bug, bool val)
{
    if (!m_data)
        return;
    switch(bug)
    {
        case HornetBug:      setFlag(0x08F6, 0x80, val); break;
        case ButterflyBug:   setFlag(0x09F2, 0x80, val); break;
        case DragonflyBug:   setFlag(0x09F5, 0x04, val); break;
        case FireflyBug:     setFlag(0x09F5, 0x20, val); break;
        case RhinoBeetleBug: setFlag(0x09F2, 0x08, val); break;
        case LadybugBug:     setFlag(0x09F2, 0x40, val); break;
        case SandCicadaBug:  setFlag(0x09F5, 0x02, val); break;
        case StagBeetleBug:  setFlag(0x09F5, 0x10, val); break;
        case GrasshopperBug: setFlag(0x09F2, 0x04, val); break;
        case MantisBug:      setFlag(0x09F2, 0x20, val); break;
        case AntBug:         setFlag(0x09F5, 0x01, val); break;
        case RollerBug:      setFlag(0x09F5, 0x08, val); break;
        default: return;
    }
    m_isDirty = true;
    emit modified();
}

quint32 SkywardSwordFile::bugQuantity(Bug bug) const
{
    if (!m_data)
        return 0;

    switch(bug)
    {
        case HornetBug:
            return quantity(true,  0x0A4C);
        case ButterflyBug:
            return quantity(false, 0x0A4A);
        case DragonflyBug:
            return quantity(true,  0x0A46);
        case FireflyBug:
            return quantity(false, 0x0A44);
        case RhinoBeetleBug:
            return quantity(false, 0x0A4E);
        case LadybugBug:
            return quantity(true, 0x0A4A);
        case SandCicadaBug:
            return quantity(false, 0x0A48);
        case StagBeetleBug:
            return quantity(true,  0x0A44);
        case GrasshopperBug:
            return quantity(true,  0x0A4E);
        case MantisBug:
            return quantity(false, 0x0A4C);
        case AntBug:
            return quantity(true,  0x0A48);
        case RollerBug:
            return quantity(false, 0x0A46);
        default:
            return 0;
    }
}

void SkywardSwordFile::setBugQuantity(Bug bug, quint32 val)
{
    if (!m_data)
        return;

    switch(bug)
    {
        case HornetBug:     setQuantity(true,  0x0A4C, val); break;
        case ButterflyBug:  setQuantity(false, 0x0A4A, val); break;
        case DragonflyBug:  setQuantity(true,  0x0A46, val); break;
        case FireflyBug:    setQuantity(false, 0x0A44, val); break;
        case RhinoBeetleBug:setQuantity(false, 0x0A4E, val); break;
        case LadybugBug:    setQuantity(true,  0x0A4A, val); break;
        case SandCicadaBug: setQuantity(false, 0x0A48, val); break;
        case StagBeetleBug: setQuantity(true,  0x0A44, val); break;
        case GrasshopperBug:setQuantity(true,  0x0A4E, val); break;
        case MantisBug:     setQuantity(false, 0x0A4C, val); break;
        case AntBug:        setQuantity(true,  0x0A48, val); break;
        case RollerBug:     setQuantity(false, 0x0A46, val); break;
        default: return;
    }
    m_isDirty = true;
    emit modified();
}

bool SkywardSwordFile::material(Material material)
{
    if (!m_data)
        return false;

    switch(material)
    {
        case HornetLarvaeMaterial:
            return flag(0x0934, 0x02);
        case BirdFeatherMaterial:
            return flag(0x0934, 0x04);
        case TumbleWeedMaterial:
            return flag(0x0934, 0x08);
        case LizardTailMaterial:
            return flag(0x0934, 0x10);
        case EldinOreMaterial:
            return flag(0x0934, 0x20);
        case AncientFlowerMaterial:
            return flag(0x0934, 0x40);
        case AmberRelicMaterial:
            return flag(0x0934, 0x80);
        case DuskRelicMaterial:
            return flag(0x0937, 0x01);
        case JellyBlobMaterial:
            return flag(0x0937, 0x02);
        case MonsterClawMaterial:
            return flag(0x0937, 0x04);
        case MonsterHornMaterial:
            return flag(0x0937, 0x08);
        case OrnamentalSkullMaterial:
            return flag(0x0937, 0x10);
        case EvilCrystalMaterial:
            return flag(0x0937, 0x20);
        case BlueBirdFeatherMaterial:
            return flag(0x0937, 0x40);
        case GoldenSkullMaterial:
            return flag(0x0937, 0x80);
        case GoddessPlumeMaterial:
            return flag(0x0936, 0x01);
        default:
            return false;
    }
}

void SkywardSwordFile::setMaterial(Material material, bool val)
{
    if (!m_data)
        return;

    switch(material)
    {
        case HornetLarvaeMaterial:    setFlag(0x0934, 0x02, val); break;
        case BirdFeatherMaterial:     setFlag(0x0934, 0x04, val); break;
        case TumbleWeedMaterial:      setFlag(0x0934, 0x08, val); break;
        case LizardTailMaterial:      setFlag(0x0934, 0x10, val); break;
        case EldinOreMaterial:        setFlag(0x0934, 0x20, val); break;
        case AncientFlowerMaterial:   setFlag(0x0934, 0x40, val); break;
        case AmberRelicMaterial:      setFlag(0x0934, 0x80, val); break;
        case DuskRelicMaterial:       setFlag(0x0937, 0x01, val); break;
        case JellyBlobMaterial:       setFlag(0x0937, 0x02, val); break;
        case MonsterClawMaterial:     setFlag(0x0937, 0x04, val); break;
        case MonsterHornMaterial:     setFlag(0x0937, 0x08, val); break;
        case OrnamentalSkullMaterial: setFlag(0x0937, 0x10, val); break;
        case EvilCrystalMaterial:     setFlag(0x0937, 0x20, val); break;
        case BlueBirdFeatherMaterial: setFlag(0x0937, 0x40, val); break;
        case GoldenSkullMaterial:     setFlag(0x0937, 0x80, val); break;
        case GoddessPlumeMaterial:    setFlag(0x0936, 0x01, val); break;
        default: return;
    }
    m_isDirty = true;
    emit modified();
}

quint32 SkywardSwordFile::materialQuantity(Material material)
{
    if (!m_data)
        return 0;

    switch(material)
    {
        case HornetLarvaeMaterial:
            return quantity(true, 0x0A42);
        case BirdFeatherMaterial:
            return quantity(false,0x0A42);
        case TumbleWeedMaterial:
            return quantity(true, 0x0A40);
        case LizardTailMaterial:
            return quantity(false,0x0A40);
        case EldinOreMaterial:
            return quantity(true, 0x0A3E);
        case AncientFlowerMaterial:
            return quantity(false,0x0A3E);
        case AmberRelicMaterial:
            return quantity(true, 0x0A3C);
        case DuskRelicMaterial:
            return quantity(false,0x0A3C);
        case JellyBlobMaterial:
            return quantity(true, 0x0A3A);
        case MonsterClawMaterial:
            return quantity(false,0x0A3A);
        case MonsterHornMaterial:
            return quantity(true, 0x0A38);
        case OrnamentalSkullMaterial:
            return quantity(false,0x0A38);
        case EvilCrystalMaterial:
            return quantity(true, 0x0A36);
        case BlueBirdFeatherMaterial:
            return quantity(false,0x0A36);
        case GoldenSkullMaterial:
            return quantity(true, 0x0A34);
        case GoddessPlumeMaterial:
            return quantity(false,0x0A34);
        default:
            return 0;
    }
}

void SkywardSwordFile::setMaterialQuantity(Material material, quint32 val)
{
    if (!m_data)
        return;

    switch(material)
    {
        case HornetLarvaeMaterial:   setQuantity(true,  0x0A42, val); break;
        case BirdFeatherMaterial:    setQuantity(false, 0x0A42, val); break;
        case TumbleWeedMaterial:     setQuantity(true,  0x0A40, val); break;
        case LizardTailMaterial:     setQuantity(false, 0x0A40, val); break;
        case EldinOreMaterial:       setQuantity(true,  0x0A3E, val); break;
        case AncientFlowerMaterial:  setQuantity(false, 0x0A3E, val); break;
        case AmberRelicMaterial:     setQuantity(true,  0x0A3C, val); break;
        case DuskRelicMaterial:      setQuantity(false, 0x0A3C, val); break;
        case JellyBlobMaterial:      setQuantity(true,  0x0A3A, val); break;
        case MonsterClawMaterial:    setQuantity(false, 0x0A3A, val); break;
        case MonsterHornMaterial:    setQuantity(true,  0x0A38, val); break;
        case OrnamentalSkullMaterial:setQuantity(false, 0x0A38, val); break;
        case EvilCrystalMaterial:    setQuantity(true,  0x0A36, val); break;
        case BlueBirdFeatherMaterial:setQuantity(false, 0x0A36, val); break;
        case GoldenSkullMaterial:    setQuantity(true,  0x0A34, val); break;
        case GoddessPlumeMaterial:   setQuantity(false, 0x0A34, val); break;
        default: return;
    }
    m_isDirty = true;
    emit modified();
}

quint32 SkywardSwordFile::gratitudeCrystalAmount()
{
    if (!m_data)
        return 0;
    quint32 ret = (quint32)((qFromBigEndian<quint16>(*(quint16*)(m_data + gameOffset() + 0x0A50)) >> 3) & 127);
    return ret;
}

void SkywardSwordFile::setGratitudeCrystalAmount(quint16 val)
{
    if (!m_data)
        return;
    quint16 oldVal = qFromBigEndian<quint16>(*(quint16*)(m_data + gameOffset() + 0x0A50)) & 0xFC00;
    *(quint16*)(m_data + gameOffset() + 0x0A50) = qToBigEndian<quint16>(oldVal | (val << 3 & 0x03FF));
    m_isDirty = true;
    this->updateChecksum();
    emit modified();
}

ushort SkywardSwordFile::rupees() const
{
    if (!m_data)
        return 0;

    return qFromBigEndian<quint16>(*(ushort*)(m_data + gameOffset() + 0x0A5E));
}

void SkywardSwordFile::setRupees(int val)
{
    if (!m_data)
        return;
    *(ushort*)(m_data + gameOffset() + 0x0A5E) = qToBigEndian<quint16>((ushort)val);
    m_isDirty = true;
    this->updateChecksum();
    emit modified();
}

ushort SkywardSwordFile::totalHP() const
{
    if (!m_data)
        return 0;
    return qToBigEndian<quint16>(*(ushort*)(m_data + gameOffset() + 0x5302));
}

void SkywardSwordFile::setTotalHP(int val)
{
    if (!m_data)
        return;

    *(ushort*)(m_data + gameOffset() + 0x5302) = qToBigEndian<quint16>((ushort)val);
    m_isDirty = true;
    emit modified();
}

ushort SkywardSwordFile::unkHP() const
{
    if (!m_data)
        return 0;

    return qFromBigEndian<quint16>(*(ushort*)(m_data + gameOffset() + 0x5304));
}

void SkywardSwordFile::setUnkHP(int val)
{
    if (!m_data)
        return;

    *(ushort*)(m_data + gameOffset() + 0x5304) = qToBigEndian<quint16>((ushort)val);
    m_isDirty = true;
    emit modified();
}

ushort SkywardSwordFile::currentHP() const
{
    if (!m_data)
        return 0;

    return qFromBigEndian<quint16>(*(quint16*)(m_data + gameOffset() + 0x5306));
}

void SkywardSwordFile::setCurrentHP(int val)
{
    if (!m_data)
        return;
    *(ushort*)(m_data + gameOffset() + 0x5306) = qToBigEndian<quint16>((ushort)val);
    m_isDirty = true;
    emit modified();
}

uint SkywardSwordFile::roomID() const
{
    return (uint)(*(uchar*)(m_data + gameOffset() + 0x5309));
}

void SkywardSwordFile::setRoomID(int val)
{
    *(uchar*)(m_data + gameOffset() + 0x5309) = (uchar)val;
    m_isDirty = true;
    this->updateChecksum();
    emit modified();
}

QString SkywardSwordFile::currentMap() const
{
    return readNullTermString(gameOffset() + 0x531c);
}

void SkywardSwordFile::setCurrentMap(const QString& map)
{
    writeNullTermString(map, gameOffset() + 0x531c);
    m_isDirty = true;
    this->updateChecksum();
    emit modified();
}

QString SkywardSwordFile::currentArea() const
{
    return readNullTermString(gameOffset() + 0x533c);
}

void SkywardSwordFile::setCurrentArea(const QString& map)
{
    writeNullTermString(map, gameOffset() + 0x533c);
    m_isDirty = true;
    this->updateChecksum();
    emit modified();
}

QString SkywardSwordFile::currentRoom() const // Not sure about this one
{
    return readNullTermString(gameOffset() + 0x535c);
}

void SkywardSwordFile::setCurrentRoom(const QString& map) // Not sure about this one
{
    writeNullTermString(map, gameOffset() + 0x535c);
    m_isDirty = true;
    this->updateChecksum();
    emit modified();
}

void SkywardSwordFile::setGameData(const QByteArray &data)
{
    if (!m_data)
        return;

    memcpy(m_data + gameOffset(), data.data(), data.size());
}

QByteArray SkywardSwordFile::gameData()
{
    if (!m_data)
        return QByteArray(0x53C0, 0);

    return QByteArray(m_data + gameOffset(), 0x53BC);
}

quint8* SkywardSwordFile::skipData() const
{
    if (!m_data)
        return NULL;
    quint8* skip = new quint8[0x80];
    memcpy(skip, (m_data + 0x20 + (0x53C0 * 3)), 0x80);

    return skip;
}

void SkywardSwordFile::setSkipData(const quint8 *data)
{
    memcpy((m_data + 0x20 + (0x53C0 * 3)), data, 0x80);
    m_isDirty = true;
    emit modified();
}

uint SkywardSwordFile::checksum() const
{
    if (!m_data)
        return 0;

    return qFromBigEndian<quint32>(*(quint32*)(m_data + gameOffset() + 0x53bc));
}

uint SkywardSwordFile::gameOffset() const
{
    if (!m_data)
        return 0;

    return (0x20 + (0x53C0 * m_game));
}

void SkywardSwordFile::updateChecksum()
{
    if (!m_data)
        return;

    quint32 checksum = m_checksumEngine.CRC32((const unsigned char*)m_data, gameOffset(), 0x53BC);
    if (this->checksum() != checksum)
    {
        *(uint*)(m_data + gameOffset() + 0x53BC) =  qToBigEndian<quint32>(checksum); // change it to Big Endian
        emit checksumUpdated();
    }
}

bool SkywardSwordFile::isNew() const
{
    if (!m_data)
        return true;

    return (*(char*)(m_data + gameOffset() + 0x53AD)) != 0;
}

void SkywardSwordFile::setNew(bool val)
{
    *(char*)(m_data + gameOffset() + 0x53AD) = val;
    m_isDirty = true;
    emit modified();
}

bool SkywardSwordFile::isModified() const
{
    return m_isDirty;
}

QString SkywardSwordFile::readNullTermString(int offset) const
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

void SkywardSwordFile::writeDataFile(const QString &filepath, char* data, quint64 len)
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

void SkywardSwordFile::writeNullTermString(const QString& val, int offset)
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

bool SkywardSwordFile::flag(quint32 offset, quint32 flag) const
{
    return (*(char*)(m_data + gameOffset() + offset) & flag) == flag;
}

void SkywardSwordFile::setFlag(quint32 offset, quint32 flag, bool val)
{
    if (val)
        *(char*)(m_data + gameOffset() + offset) |= flag;
    else
        *(char*)(m_data + gameOffset() + offset) &= ~flag;
}

bool SkywardSwordFile::isValidFile(const QString &filepath, Region* outRegion)
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

// SLOTS

void SkywardSwordFile::slingshotChanged(bool val)
{
    this->setEquipment(SlingshotWeapon, val);
}

void SkywardSwordFile::scattershotChanged(bool val)
{
    this->setEquipment(ScattershotWeapon, val);
}

void SkywardSwordFile::seedAmmoQuantityChanged(int val)
{
    this->setAmmo(SeedAmmo, val);
}

void SkywardSwordFile::bugnetChanged(bool val)
{
    this->setEquipment(BugnetWeapon, val);
}

void SkywardSwordFile::bigBugnetChanged(bool val)
{
    this->setEquipment(BigBugnetWeapon, val);
}

void SkywardSwordFile::beetleChanged(bool val)
{
    this->setEquipment(BeetleWeapon, val);

}

void SkywardSwordFile::hookBeetleChanged(bool val)
{
    this->setEquipment(HookBeetleWeapon, val);
}

void SkywardSwordFile::quickBeetleChanged(bool val)
{
    this->setEquipment(QuickBeetleWeapon, val);
}

void SkywardSwordFile::toughBeetleChanged(bool val)
{
    this->setEquipment(ToughBeetleWeapon, val);
}

void SkywardSwordFile::gustBellowsChanged(bool val)
{
    this->setEquipment(GustBellowsWeapon, val);
}

void SkywardSwordFile::whipChanged(bool val)
{
    this->setEquipment(WhipWeapon, val);
}

void SkywardSwordFile::clawshotChanged(bool val)
{
    this->setEquipment(ClawshotWeapon, val);
}

void SkywardSwordFile::bombChanged(bool val)
{
    this->setEquipment(BombWeapon, val);
}

void SkywardSwordFile::bombAmmoQuantityChanged(int val)
{
    this->setAmmo(BombAmmo, val);
}

void SkywardSwordFile::bowChanged(bool val)
{
    this->setEquipment(BowWeapon, val);
}

void SkywardSwordFile::ironBowChanged(bool val)
{
    this->setEquipment(IronBowWeapon, val);
}

void SkywardSwordFile::sacredBowChanged(bool val)
{
    this->setEquipment(SacredBowWeapon, val);
}

void SkywardSwordFile::arrowAmmoQuantityChanged(int val)
{
    this->setAmmo(ArrowAmmo, val);
}

void SkywardSwordFile::harpChanged(bool val)
{
    this->setEquipment(HarpEquipment, val);
}

void SkywardSwordFile::sailClothChanged(bool val)
{
    this->setEquipment(SailClothEquipment, val);
}

void SkywardSwordFile::diggingMittsChanged(bool val)
{
    this->setEquipment(DiggingMittsEquipment, val);
}

void SkywardSwordFile::moleMittsChanged(bool val)
{
    this->setEquipment(MoleMittsEquipment, val);
}

void SkywardSwordFile::fireShieldEaringsChanged(bool val)
{
    this->setEquipment(FireShieldEaringsEquipment, val);
}

void SkywardSwordFile::waterDragonScaleChanged(bool val)
{
    this->setEquipment(WaterDragonScaleEquipment, val);
}

void SkywardSwordFile::hornetChanged(bool val)
{
    this->setBug(HornetBug, val);
}

void SkywardSwordFile::hornetQuantityChanged(int val)
{
    this->setBugQuantity(HornetBug, val);
}
void SkywardSwordFile::butterflyChanged(bool val)
{
    this->setBug(ButterflyBug, val);
}

void SkywardSwordFile::butterflyQuantityChanged(int val)
{
    this->setBugQuantity(ButterflyBug, val);
}
void SkywardSwordFile::dragonflyChanged(bool val)
{
    this->setBug(DragonflyBug, val);
}

void SkywardSwordFile::dragonflyQuantityChanged(int val)
{
    this->setBugQuantity(DragonflyBug, val);
}

void SkywardSwordFile::fireflyChanged(bool val)
{
    this->setBug(FireflyBug, val);
}

void SkywardSwordFile::fireflyQuantityChanged(int val)
{
    this->setBugQuantity(FireflyBug, val);
}

void SkywardSwordFile::rhinoBeetleChanged(bool val)
{
    this->setBug(RhinoBeetleBug, val);
}

void SkywardSwordFile::rhinoBeetleQuantityChanged(int val)
{
    this->setBugQuantity(RhinoBeetleBug, val);
}

void SkywardSwordFile::ladybugChanged(bool val)
{
    this->setBug(LadybugBug, val);
}

void SkywardSwordFile::ladybugQuantityChanged(int val)
{
    this->setBugQuantity(LadybugBug, val);
}

void SkywardSwordFile::sandCicadaChanged(bool val)
{
    this->setBug(SandCicadaBug, val);
}

void SkywardSwordFile::sandCicadaQuantityChanged(int val)
{
    this->setBugQuantity(SandCicadaBug, val);
}

void SkywardSwordFile::stagBeetleChanged(bool val)
{
    this->setBug(StagBeetleBug, val);
}

void SkywardSwordFile::stagBeetleQuantityChanged(int val)
{
    this->setBugQuantity(StagBeetleBug, val);
}

void SkywardSwordFile::grasshopperChanged(bool val)
{
    this->setBug(GrasshopperBug, val);
}

void SkywardSwordFile::grasshopperQuantityChanged(int val)
{
    this->setBugQuantity(GrasshopperBug, val);
}

void SkywardSwordFile::mantisChanged(bool val)
{
    this->setBug(MantisBug, val);
}

void SkywardSwordFile::mantisQuantityChanged(int val)
{
    this->setBugQuantity(MantisBug, val);
}

void SkywardSwordFile::antChanged(bool val)
{
    this->setBug(AntBug, val);
}

void SkywardSwordFile::antQuantityChanged(int val)
{
    this->setBugQuantity(AntBug, val);
}

void SkywardSwordFile::eldinRollerChanged(bool val)
{
    this->setBug(RollerBug, val);
}

void SkywardSwordFile::eldinRollerQuantityChanged(int val)
{
    this->setBugQuantity(RollerBug, val);
}

void SkywardSwordFile::hornetLarvaeChanged(bool val)
{
    this->setMaterial(HornetLarvaeMaterial, val);
}

void SkywardSwordFile::hornetLarvaeQuantityChanged(int val)
{
    this->setMaterialQuantity(HornetLarvaeMaterial, (quint32)val);
}

void SkywardSwordFile::birdFeatherChanged(bool val)
{
    this->setMaterial(BirdFeatherMaterial, val);
}

void SkywardSwordFile::birdFeatherQuantityChanged(int val)
{
    this->setMaterialQuantity(BirdFeatherMaterial, val);
}

void SkywardSwordFile::tumbleWeedChanged(bool val)
{
    this->setMaterial(TumbleWeedMaterial, val);
}

void SkywardSwordFile::tumbleWeedQuantityChanged(int val)
{
    this->setMaterialQuantity(TumbleWeedMaterial, val);
}

void SkywardSwordFile::lizardTailChanged(bool val)
{
    this->setMaterial(LizardTailMaterial, val);
}

void SkywardSwordFile::lizardTailQuantityChanged(int val)
{
    this->setMaterialQuantity(LizardTailMaterial, val);
}

void SkywardSwordFile::eldinOreChanged(bool val)
{
    this->setMaterial(EldinOreMaterial, val);
}

void SkywardSwordFile::eldinOreQuantityChanged(int val)
{
    this->setMaterialQuantity(EldinOreMaterial, val);
}

void SkywardSwordFile::ancientFlowerChanged(bool val)
{
    this->setMaterial(AncientFlowerMaterial, val);
}

void SkywardSwordFile::ancientFlowerQuantityChanged(int val)
{
    this->setMaterialQuantity(AncientFlowerMaterial, val);
}

void SkywardSwordFile::amberRelicChanged(bool val)
{
    this->setMaterial(AmberRelicMaterial, val);
}

void SkywardSwordFile::amberRelicQuantityChanged(int val)
{
    this->setMaterialQuantity(AmberRelicMaterial, val);
}

void SkywardSwordFile::duskRelicChanged(bool val)
{
    this->setMaterial(DuskRelicMaterial, val);
}

void SkywardSwordFile::duskRelicQuantityChanged(int val)
{
    this->setMaterialQuantity(DuskRelicMaterial, val);
}

void SkywardSwordFile::jellyBlobChanged(bool val)
{
    this->setMaterial(JellyBlobMaterial, val);
}

void SkywardSwordFile::jellyBlobQuantityChanged(int val)
{
    this->setMaterialQuantity(JellyBlobMaterial, val);
}

void SkywardSwordFile::monsterClawChanged(bool val)
{
    this->setMaterial(MonsterClawMaterial, val);
}

void SkywardSwordFile::monsterClawQuantityChanged(int val)
{
    this->setMaterialQuantity(MonsterClawMaterial, val);
}

void SkywardSwordFile::monsterHornChanged(bool val)
{
    this->setMaterial(MonsterHornMaterial, val);
}

void SkywardSwordFile::monsterHornQuantityChanged(int val)
{
    this->setMaterialQuantity(MonsterHornMaterial, val);
}

void SkywardSwordFile::decoSkullChanged(bool val)
{
    this->setMaterial(OrnamentalSkullMaterial, val);
}

void SkywardSwordFile::decoSkullQuantityChanged(int val)
{
    this->setMaterialQuantity(OrnamentalSkullMaterial, val);
}

void SkywardSwordFile::evilCrystalChanged(bool val)
{
    this->setMaterial(EvilCrystalMaterial, val);
}

void SkywardSwordFile::evilCrystalQuantityChanged(int val)
{
    this->setMaterialQuantity(EvilCrystalMaterial, val);
}

void SkywardSwordFile::blueBirdFeatherChanged(bool val)
{
    this->setMaterial(BlueBirdFeatherMaterial, val);
}

void SkywardSwordFile::blueBirdFeatherQuantityChanged(int val)
{
    this->setMaterialQuantity(BlueBirdFeatherMaterial, val);
}

void SkywardSwordFile::goldenSkullChanged(bool val)
{
    this->setMaterial(GoldenSkullMaterial, val);
}

void SkywardSwordFile::goldenSkullQuantityChanged(int val)
{
    this->setMaterialQuantity(GoldenSkullMaterial, val);
}

void SkywardSwordFile::goddessPlumeChanged(bool val)
{
    this->setMaterial(GoddessPlumeMaterial, val);
}

void SkywardSwordFile::goddessPlumeQuantityChanged(int val)
{
    this->setMaterialQuantity(GoddessPlumeMaterial, val);
}

void SkywardSwordFile::gratitudeCrystalAmountChanged(int val)
{
    this->setGratitudeCrystalAmount(val);
}

quint32 SkywardSwordFile::quantity(bool isRight, int offset) const
{
    if (!m_data)
        return 0;
    switch(isRight)
    {
        case false:
            return (quint32)(qFromBigEndian<quint16>((*(quint16*)(m_data + gameOffset() + offset))) >> 7) & 127;
        case true:
            return (quint32)(qFromBigEndian<quint16>(*(quint16*)(m_data + gameOffset() + offset))) & 127;
    }

    return 0;
}

void SkywardSwordFile::setQuantity(bool isRight, int offset, quint32 val)
{
    if (!m_data)
        return;

    quint16 oldVal = qFromBigEndian<quint16>(*(quint16*)(m_data + gameOffset() + offset));
    switch(isRight)
    {
        case false:
        {
            quint16 newVal = (oldVal&127)|(((quint16)val << 7));
            *(quint16*)(m_data + gameOffset() + offset) = qToBigEndian<quint16>(newVal);
        }
        break;
        case true:
        {
            oldVal = (oldVal >> 7) & 127;
            quint16 newVal = (val|(oldVal << 7));
            *(quint16*)(m_data + gameOffset() + offset) = qToBigEndian<quint16>(newVal);
        }
        break;
    }
}

bool SkywardSwordFile::isNight() const
{
    return (*(quint8*)(m_data + gameOffset() + 0x53B3) & 0x01) == 0x01;
}

void SkywardSwordFile::setNight(const bool val)
{
    if (val)
        *(quint8*)(m_data + gameOffset() + 0x53B3) |= 0x01;
    else
        *(quint8*)(m_data + gameOffset() + 0x53B3) &= ~0x01;

    this->updateChecksum();
    emit modified();
}

void SkywardSwordFile::setData(char *data)
{
    if (m_data)
        delete[] m_data;

    m_data = data;
    m_isOpen = true;
    this->updateChecksum();
    emit modified();
}

bool SkywardSwordFile::loadDataBin(const QString& filepath, Game game)
{
    if (!filepath.isEmpty())
        m_filename = filepath;

    m_isDirty = false;
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
            updateChecksum();
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
        QMessageBox msg(QMessageBox::Warning, "Error loading file", e.message().c_str());
        msg.exec();
    }
    catch (std::string what)
    {
        QMessageBox msg(QMessageBox::Warning, "Error loading file", what.c_str());
        msg.exec();
    }

    return false;
}

bool SkywardSwordFile::saveDataBin()
{
    if (!WiiKeys::instance()->isOpen() || !WiiKeys::instance()->isValid())
    {
        QMessageBox msg(QMessageBox::Warning, "Invalid or Missing Keys", "Required keys are either missing or invalid\nPlease check:\nEdit->Preferences\nTo ensure you have valid keys.");
        msg.exec();
        return false;
    }

    m_isDirty = false;
    if (m_saveGame != NULL)
    {
        //qDebug() << "Changing wiiking2.sav data";
        //WiiFile* wiiking2 = m_saveGame->getFile("/wiiking2.sav");
        //wiiking2->setData((unsigned char*)m_data);
        qDebug() << "Done saving to" << m_filename;
        m_saveGame->saveToFile(m_filename.toStdString(), (quint8*)WiiKeys::instance()->macAddr().data(), WiiKeys::instance()->NGID(),(quint8*)WiiKeys::instance()->NGPriv().data(), (quint8*)WiiKeys::instance()->NGSig().data(), WiiKeys::instance()->NGKeyID());
        return true;
    }
    else
    {
        m_saveGame = new WiiSave();
        int r = region();
        char gameId[5];
        memset(gameId, 0, 5);
        memcpy(gameId, (char*)&r, 4);
        qDebug() << "Attempting to load embedded banner.tpl";
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
            quint64 fullId = ((quint64)r << 32)  | qToBigEndian(titleId) >> 32;
            wiiBanner->setGameID(qFromBigEndian(fullId));

            qDebug() << "Got Banner.tpl";
            qDebug() << "Attempting to load embedded icon.tpl";
            QFile icon(":/BannerData/icon.tpl");
            if (icon.open(QFile::ReadOnly))
            {
                QDataStream dataStream(&icon);
                quint8* iconData = new quint8[48*48*2];
                dataStream.readRawData((char*)iconData, 48*48*2);
                icon.close();
                wiiBanner->addIcon(new WiiImage(48, 48, iconData));
                qDebug() << "Got icon.tpl";
            }
            else
            {
                qWarning() << "Failed to load icon.tpl bailing out!";
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
            m_saveGame->addFile("/skip.dat", new WiiFile("skip.dat", WiiFile::GroupRW | WiiFile::OwnerRW, skipData(), 0x80));
            m_saveGame->saveToFile(m_filename.toStdString(), (quint8*)WiiKeys::instance()->macAddr().data(), WiiKeys::instance()->NGID(),(quint8*)WiiKeys::instance()->NGPriv().data(), (quint8*)WiiKeys::instance()->NGSig().data(), WiiKeys::instance()->NGKeyID());
            delete m_saveGame;
            m_saveGame = NULL;
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

QString SkywardSwordFile::bannerTitle() const
{
    if (m_saveGame != NULL)
    {
        return QString::fromUtf8(m_saveGame->banner()->title().c_str());
    }

    int r = region();
    char gameId[5];

    memset(gameId, 0, 5);
    memcpy(gameId, (char*)&r, 4);
    QFile title(QString(":/BannerData/%1/title.bin").arg(gameId));
    if (title.open(QFile::ReadOnly))
    {
        QString titleString = QString::fromUtf16((ushort*)title.readAll().data() + '\0');
        title.close();
        return titleString;
    }
    return QString("");
}

QString SkywardSwordFile::bannerSubtitle() const
{
    if (m_saveGame != NULL)
    {
        return QString::fromUtf8(m_saveGame->banner()->subtitle().c_str());
    }

    int r = region();
    char gameId[5];

    memset(gameId, 0, 5);
    memcpy(gameId, (char*)&r, 4);
    QFile subtitle(QString(":/BannerData/%1/subtitle.bin").arg(gameId));
    if (subtitle.open(QFile::ReadOnly))
    {
        QString titleString = QString::fromUtf16((ushort*)subtitle.readAll().data() + '\0');
        subtitle.close();
        return titleString;
    }
    return QString("");
}

const QIcon SkywardSwordFile::icon() const
{
    if (!m_saveGame)
    {
        QFile icon(":/BannerData/icon.tpl");
        if (icon.open(QFile::ReadOnly))
        {
            QDataStream dataStream(&icon);
            char* iconData = new char[48*48*2];
            dataStream.readRawData(iconData, 48*48*2);
            icon.close();

            return QIcon(QPixmap::fromImage(convertTextureToImage(QByteArray(iconData, 48*48*2), 48, 48)));
        }
        return QIcon();
    }

    WiiImage* icon = m_saveGame->banner()->getIcon(0);
    if (!icon)
        return QIcon();
    QIcon iconImage(QPixmap::fromImage(convertTextureToImage(QByteArray((char*)icon->data(), icon->width()*icon->height()*2), icon->width(), icon->height())));
    return iconImage;
}

const QPixmap SkywardSwordFile::banner() const
{
    if (!m_saveGame)
    {
        QFile banner(":/BannerData/banner.tpl");
        if (banner.open(QFile::ReadOnly))
        {
            QDataStream dataStream(&banner);
            char* bannerData = new char[192*168*2];
            dataStream.readRawData(bannerData, 192*168*2);
            banner.close();

            return QPixmap::fromImage(convertTextureToImage(QByteArray(bannerData, 192*64*2), 192, 64));
        }
        return QPixmap();
    }

    WiiImage* banner = m_saveGame->banner()->bannerImage();
    return QPixmap::fromImage(convertTextureToImage(QByteArray((char*)banner->data(), banner->width()*banner->height()*2), banner->width(), banner->height()));
}

// To support MSVC I have placed these here, why can't Microsoft follow real ANSI Standards? <.<
const float SkywardSwordFile::DEFAULT_POS_X = -4798.150391f;
const float SkywardSwordFile::DEFAULT_POS_Y =  1237.629517f;
const float SkywardSwordFile::DEFAULT_POS_Z = -6573.722656f;

