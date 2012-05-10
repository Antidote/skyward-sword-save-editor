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

#ifndef GAMEFILE_H
#define GAMEFILE_H

#include <QFile>
#include "CRC32.h"

class QDateTime;


#define TICKS_PER_SECOND 60750000
#define SECONDS_TO_2000  946684800

struct PlayTime
{
    int Hours;
    int Minutes;
    int Seconds;
    quint64 RawTicks;
};

class GameFile
{
public:
    // Default coords for new file
    static const float DEFAULT_POS_X = -4798.150391;
    static const float DEFAULT_POS_Y =  1237.629517;
    static const float DEFAULT_POS_Z = -6573.722656;

    enum Game
    {
        GameNone = -1,
        Game1,
        Game2,
        Game3
    };

    enum Region
    {
        NTSCURegion = 0x45554F53,
        NTSCJRegion = 0x4A554F53,
        PALRegion   = 0x50554F53
    };
    enum Bug
    {
        HornetBug,
        GrasshopperBug,
        LadybugBug, // o.o that looks weird
        ButterflyBug,
        MantisBug,
        BeetleBug
    };

    enum Weapon
    {
        SlingshotWeapon,
        ScattershotWeapon,
        BugnetWeapon,
        BigBugnetWeapon,
        BeetleWeapon,
        HookBeetleWeapon,
        QuickBeetleWeapon,
        ToughBeetleWeapon,
        BombWeapon,
        GustBellowsWeapon,
        WhipWeapon,
        ClawshotWeapon,
        BowWeapon,
        IronBowWeapon,
        SacredBowWeapon
    };

    enum Equipment
    {
        HarpEquipment
    };

    enum Sword
    {
        PracticeSword,
        GoddessSword,
        LongSword,
        WhiteSword,
        MasterSword,
        TrueMasterSword
    };

    GameFile(const QString& filepath = NULL, Game game = Game1);
    ~GameFile();

    bool Save(const QString& filename = NULL);
    bool Open(Game game = GameNone, const QString& filepath=NULL);
    void CreateNewGame(Game game);
    void DeleteGame(Game game = GameNone);
    void DeleteAllGames();
    void UpdateChecksum();
    bool HasValidChecksum(); // for integrity checks
    bool IsModified() const;

    /// HasFileOnDiskChanged
    /// Checks against the stored checksum for the entire file,
    /// if the file on disk differs from the stored checksum,
    /// the file has changed (should probably use an SHA1 or MD5 hash for the but w/e).
    bool HasFileOnDiskChanged();
    void Close(); //<! Closes the current file without saving.
    void Reload(Game game);

    bool IsOpen() const;

    QString GetFilename() const;
    void SetFilename(const QString& filepath);

    Game GetGame() const;
    void SetGame(Game game);

    uint GetGameOffset() const;

    Region GetRegion() const;
    void  SetRegion(Region);
    PlayTime GetPlayTime() const;
    void  SetPlayTime(PlayTime val);
    QDateTime GetSaveTime() const;
    void  SetSaveTime(QDateTime val);
    float GetPlayerX() const;
    void  SetPlayerX(float val);
    float GetPlayerY() const;
    void  SetPlayerY(float val);
    float GetPlayerZ() const;
    void  SetPlayerZ(float val);
    float GetPlayerRoll() const;
    void  SetPlayerRoll(float val);
    float GetPlayerPitch() const;
    void  SetPlayerPitch(float val);
    float GetPlayerYaw() const;
    void  SetPlayerYaw(float val);
    float GetCameraX() const;
    void  SetCameraX(float val);
    float GetCameraY() const;
    void  SetCameraY(float val);
    float GetCameraZ() const;
    void  SetCameraZ(float val);
    float GetCameraRoll() const;
    void  SetCameraRoll(float val);
    float GetCameraPitch() const;
    void  SetCameraPitch(float val);
    float GetCameraYaw() const;
    void  SetCameraYaw(float val);
    QString GetPlayerName() const;
    void SetPlayerName(const QString& name);
    bool IsHeroMode() const;
    void SetHeroMode(bool val);
    bool GetIntroViewed() const;
    void SetIntroViewed(bool val);
    bool GetSword(Sword sword) const;
    void SetSword(Sword sword, bool val);
    bool GetWeapon(Weapon weapon) const;
    void SetWeapon(Weapon, bool val);
    bool GetBug(Bug bug) const;
    void SetBug(Bug bug, bool val);
    ushort GetRupees() const;
    void SetRupees(ushort val);
    ushort GetTotalHP() const;
    void SetTotalHP(ushort val);
    ushort GetUnkHP() const;
    void SetUnkHP(ushort val);
    ushort GetCurrentHP() const;
    void SetCurrentHP(ushort val);
    uint GetChecksum() const;
    uint  GetRoomID() const;
    void SetRoomID(uint val);
    QString GetCurrentMap() const;
    void SetCurrentMap(const QString& map);
    QString GetCurrentArea() const;
    void SetCurrentArea(const QString& map);
    QString GetCurrentRoom() const;
    void SetCurrentRoom(const QString& map);

    bool IsNew() const;
    void SetNew(bool val);

private:
    QString ReadNullTermString(int offset) const;
    void WriteNullTermString(const QString& val, int offset);
    bool GetFlag(quint32 offset, quint32 flag) const;
    void SetFlag(quint32 offset, quint32 flag, bool val);
    char*    m_data;
    QString  m_filename;
    Game     m_game;
    bool     m_isOpen;
    quint32  m_fileChecksum; // The checksum of the entire file.
    CRC32*   m_crcEngine;
};

#endif // GAMEFILE_H
