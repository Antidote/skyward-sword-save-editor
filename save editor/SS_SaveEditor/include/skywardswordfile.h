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

#ifndef SKYWARDSWORDFILE_H
#define SKYWARDSWORDFILE_H

#include <QFile>
#include "igamefile.h"
#include <QImage>
#include "WiiQt/savedatabin.h"
#include "WiiQt/savebanner.h"

class QDateTime;
class Checksum;
struct SaveGame;



#define TICKS_PER_SECOND 60750000
#define SECONDS_TO_2000  946684800

struct PlayTime
{
    int Hours;
    int Minutes;
    int Seconds;
    quint64 RawTicks;
};

struct Vector3
{
    float X;
    float Y;
    float Z;

    Vector3(float x, float y, float z) : X(x), Y(y), Z(z)
    {}
};

class SkywardSwordFile : public IGameFile
{
public:
    // Default coords for new file
    static const float DEFAULT_POS_X;
    static const float DEFAULT_POS_Y;
    static const float DEFAULT_POS_Z;

    enum Region
    {
                 NTSCURegion = 0x45554F53,
                 NTSCJRegion = 0x4A554F53,
                 PALRegion   = 0x50554F53
    };
    enum Bug
    {
                 HornetBug,
                 ButterflyBug,
                 DragonflyBug,
                 FireflyBug,
                 RhinoBeetleBug,
                 LadybugBug, // o.o that looks weird
                 SandCicadaBug,
                 StagBeetleBug,
                 GrasshopperBug,
                 MantisBug,
                 AntBug,
                 RollerBug
    };

    enum WeaponEquipment
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
                 SacredBowWeapon,
                 HarpEquipment,
                 SailClothEquipment,
                 DiggingMitts,
                 MoleMittsEquipment,
                 FireShieldEaringsEquipment,
                 DragonScaleEquipment
    };

    enum Material
    {
                 HornetLarvaeMaterial,
                 BirdFeatherMaterial,
                 TumbleWeedMaterial,
                 LizardTailMaterial,
                 OreMaterial,
                 AncientFlowerMaterial,
                 AmberRelicMaterial,
                 DuskRelicMaterial,
                 JellyBlobMaterial,
                 MonsterClawMaterial,
                 MonsterHornMaterial,
                 OrnamentalSkullMaterial,
                 EvilCrystalMaterial,
                 BlueBirdFeatherMaterial,
                 GoldenSkullMaterial,
                 GoddessPlumeMaterial
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

    SkywardSwordFile(const QString& filepath = NULL, Game game = Game1);
    ~SkywardSwordFile();

    bool Save(const QString& filename = NULL);
    bool Open(Game game = GameNone, const QString& filepath=NULL);
    void CreateNewGame(Game game);
    void ExportGame(const QString& filepath, Game game = GameNone);
    void ExportGame(const QString& filepath, Game game = GameNone, Region region = NTSCURegion);
    void DeleteGame(Game game = GameNone);
    void DeleteAllGames();
    void UpdateChecksum();
    bool HasValidChecksum(); // for integrity checks
    bool IsModified() const;

    /// HasFileOnDiskChanged
    /// Checks against the stored checksum for the entire file,
    /// if the file on disk differs from the stored checksum,
    /// the file has changed (should probably use an SHA1 or MD5 hash for the but w/e).
    bool            HasFileOnDiskChanged();
    void            Close(); //<! Closes the current file without saving.
    void            Reload(Game game);

    bool            IsOpen() const;

    QString   GetFilename() const;
    void            SetFilename(const QString& filepath);

    Game            GetGame() const;
    void            SetGame(Game game);

    Region    GetRegion() const;
    void      SetRegion(Region);
    PlayTime  GetPlayTime() const;
    void      SetPlayTime(PlayTime val);
    QDateTime GetSaveTime() const;
    void      SetSaveTime();
    Vector3   GetPlayerPosition() const;
    void      SetPlayerPosition(float x, float y, float z);
    void      SetPlayerPosition(Vector3 pos);
    Vector3   GetPlayerRotation() const;
    void      SetPlayerRotation(float roll, float pitch, float yaw);
    void      SetPlayerRotation(Vector3 rotation);
    Vector3   GetCameraPosition() const;
    void      SetCameraPosition(float x, float y, float z);
    void      SetCameraPosition(Vector3 position);
    Vector3   GetCameraRotation() const;
    void      SetCameraRotation(float roll, float pitch, float yaw);
    void      SetCameraRotation(Vector3 rotation);
    QString   GetPlayerName() const;
    void      SetPlayerName(const QString& name);
    bool      IsHeroMode() const;
    void      SetHeroMode(bool val);
    bool      GetIntroViewed() const;
    void      SetIntroViewed(bool val);
    bool      GetSword(Sword sword) const;
    void      SetSword(Sword sword, bool val);
    bool      GetEquipment(WeaponEquipment weapon) const;
    void      SetEquipment(WeaponEquipment, bool val);
    bool      GetBug(Bug bug) const;
    void      SetBug(Bug bug, bool val);
    quint32   GetBugQuantity(Bug bug) const;
    void      SetBugQuantity(Bug bug, quint32 val);
    bool      GetMaterial(Material material);
    void      SetMaterial(Material material, bool val);
    quint32   GetGratitudeCrystalAmount();
    void      SetGratitudeCrystalAmount(quint32 val);
    ushort    GetRupees() const;
    void      SetRupees(ushort val);
    ushort    GetTotalHP() const;
    void      SetTotalHP(ushort val);
    ushort    GetUnkHP() const;
    void      SetUnkHP(ushort val);
    ushort    GetCurrentHP() const;
    void      SetCurrentHP(ushort val);
    uint      GetChecksum() const;
    uint      GetRoomID() const;
    void      SetRoomID(uint val);
    QString   GetCurrentMap() const;
    void      SetCurrentMap(const QString& map);
    QString   GetCurrentArea() const;
    void      SetCurrentArea(const QString& map);
    QString   GetCurrentRoom() const;
    void      SetCurrentRoom(const QString& map);

    bool      IsNew() const;
    void      SetNew(bool val);

    void      SetData(char* data);
    bool      LoadDataBin(const QString& filepath = NULL, Game game = Game1);
    bool      CreateDataBin();
    QString   GetBannerTitle() const;
    const QImage& GetBanner() const;
    static bool IsValidFile(const QString& filepath, Region* region);

    quint32 GetQuantity(bool isRight, int offset) const;
    void    SetQuantity(bool isRight, int offset, quint32 val);
private:
    uint    GetGameOffset() const;
    QString ReadNullTermString(int offset) const;
    void    WriteNullTermString(const QString& val, int offset);
    bool    GetFlag(quint32 offset, quint32 flag) const;
    void    SetFlag(quint32 offset, quint32 flag, bool val);
    char*   m_data;
    QImage  m_bannerImage;
    QString m_filename;
    Game    m_game;
    bool    m_isOpen;
    quint32 m_fileChecksum; // The checksum of the entire file.
    SaveGame m_saveGame;
    SaveDataBin m_dataBin;
    SaveBanner  m_banner;
    Checksum*  m_checksumEngine;
};

#endif // GAMEFILE_H
