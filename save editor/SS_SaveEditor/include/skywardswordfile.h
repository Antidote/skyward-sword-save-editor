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

#include <QObject>

#include <QFile>
#include "igamefile.h"
#include <QImage>
#include <QIcon>
#include "WiiSave.h"
#include "WiiBanner.h"
#include "checksum.h"

class QDateTime;
class WiiSave;

struct PlayTime
{
    int Days;
    int Hours;
    int Minutes;
    int Seconds;
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
    Q_OBJECT
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

    enum Ammo
    {
        ArrowAmmo,
        BombAmmo,
        SeedAmmo
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
         DiggingMittsEquipment,
         MoleMittsEquipment,
         FireShieldEaringsEquipment,
         WaterDragonScaleEquipment
    };

    enum Material
    {
         HornetLarvaeMaterial,
         BirdFeatherMaterial,
         TumbleWeedMaterial,
         LizardTailMaterial,
         EldinOreMaterial,
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

    enum WalletType
    {
        SmallWallet,
        MediumWallet,
        BigWallet,
        GiantWallet,
        TycoonWallet
    };

    SkywardSwordFile(Region region);
    SkywardSwordFile(const QString& filepath = NULL, Game game = Game1);
    virtual ~SkywardSwordFile();

    bool save(const QString& filepath = "");
    bool open(Game game = GameNone, const QString& filepath="");
    void createNewGame(Game game);
    void createEmptyFile(Region region);
    void exportGame(const QString& filepath, Game game = GameNone);
    void exportGame(const QString& filepath, Game game = GameNone, Region region = NTSCURegion);
    void deleteGame(Game game = GameNone);
    void deleteAllGames();
    void updateChecksum();
    bool hasValidChecksum(); // for integrity checks
    bool isModified() const;

    void      close(); //<! Closes the current file without saving.
    bool      reload(Game game);

    bool      isOpen() const;

    QString   filename() const;
    void      setFilename(const QString& filepath);

    Game      game() const;
    void      setGame(Game game);

    Region    region() const;
    void      setRegion(Region);
    PlayTime  playTime() const;
    //void      setPlayTime(PlayTime val);
    QDateTime saveTime() const;
    Vector3   playerPosition() const;
    void      setPlayerPosition(float x, float y, float z);
    void      setPlayerPosition(Vector3 pos);
    Vector3   playerRotation() const;
    void      setPlayerRotation(float roll, float pitch, float yaw);
    void      setPlayerRotation(Vector3 rotation);
    Vector3   cameraPosition() const;
    void      setCameraPosition(float x, float y, float z);
    void      setCameraPosition(Vector3 position);
    Vector3   cameraRotation() const;
    void      setCameraRotation(float roll, float pitch, float yaw);
    void      setCameraRotation(Vector3 rotation);
    QString   playerName() const;
    bool      isHeroMode() const;
    bool      introViewed() const;
    bool      wallet(WalletType);
    bool      sword(Sword sword) const;
    void      setSword(Sword sword, bool val);
    bool      equipment(WeaponEquipment weapon) const;
    void      setEquipment(WeaponEquipment, bool val);
    quint32   ammo(Ammo type);
    void      setAmmo(Ammo type, quint32 val);
    bool      bug(Bug setBug) const;
    void      setBug(Bug setBug, bool val);
    quint32   bugQuantity(Bug setBug) const;
    void      setBugQuantity(Bug setBug, quint32 val);
    bool      material(Material material);
    void      setMaterial(Material material, bool val);
    quint32   materialQuantity(Material material);
    void      setMaterialQuantity(Material material, quint32 val);
    quint32   gratitudeCrystalAmount();
    void      setGratitudeCrystalAmount(quint16 val);
    ushort    rupees() const;
    ushort    totalHP() const;

    ushort    unkHP() const;
    ushort    currentHP() const;
    uint      checksum() const;
    uint      roomID() const;
    QString   currentMap() const;
    QString   currentArea() const;
    QString   currentRoom() const;

    bool      isNight() const;
    void      setGameData(const QByteArray& data);
    QByteArray gameData();
    quint8*   skipData() const;
    void      setSkipData(const quint8* data);

    bool      isNew() const;
    void      setNew(bool val);

    void      setData(char* data);
    bool      loadDataBin(const QString& filepath = "", Game game = Game1);
    bool      saveDataBin();
    QString   bannerTitle() const;
    QString   bannerSubtitle() const;
    const QPixmap banner() const;
    const QIcon  icon() const;
    static bool isValidFile(const QString& filepath, Region* region);

signals:
    void checksumUpdated();
    void modified();

public slots:
    // General
    void setPlayTime(PlayTime val);
    void setSaveTime(const QDateTime& time);
    void setPlayerName(const QString& name);
    void setRupees(int val);
    void setTotalHP(int val);
    void setUnkHP(int val);
    void setCurrentHP(int val);
    void setRoomID(int val);
    void setCurrentRoom(const QString& map);
    void setCurrentMap(const QString& map);
    void setCurrentArea(const QString& map);
    void setNight(const bool val);
    void setIntroViewed(bool val);
    void setHeroMode(bool val);

    // Wallets
    void setSmallWallet(bool);
    void setMediumWallet(bool);
    void setBigWallet(bool);
    void setGiantWallet(bool);
    void setTycoonWallet(bool);
    // Sword
    void practiceSwordChanged(bool);
    void goddessSwordChanged(bool);
    void goddessLongSwordChanged(bool);
    void goddessWhiteSwordChanged(bool);
    void masterSwordChanged(bool);
    void trueMasterSwordChanged(bool);
    // Equipment
    void slingshotChanged(bool);
    void scattershotChanged(bool);
    void seedAmmoQuantityChanged(int);
    void bugnetChanged(bool);
    void bigBugnetChanged(bool);
    void beetleChanged(bool);
    void hookBeetleChanged(bool);
    void quickBeetleChanged(bool);
    void toughBeetleChanged(bool);
    void gustBellowsChanged(bool);
    void whipChanged(bool);
    void clawshotChanged(bool);
    // Bombs
    void bombChanged(bool);
    void bombAmmoQuantityChanged(int);
    // Bows
    void bowChanged(bool);
    void ironBowChanged(bool);
    void sacredBowChanged(bool);
    void arrowAmmoQuantityChanged(int);
    void harpChanged(bool);
    void sailClothChanged(bool);
    void diggingMittsChanged(bool);
    void moleMittsChanged(bool);
    void fireShieldEaringsChanged(bool);
    void waterDragonScaleChanged(bool);
    // Bugs
    void hornetChanged(bool);
    void hornetQuantityChanged(int);
    void butterflyChanged(bool);
    void butterflyQuantityChanged(int);
    void dragonflyChanged(bool);
    void dragonflyQuantityChanged(int);
    void fireflyChanged(bool);
    void fireflyQuantityChanged(int);
    void rhinoBeetleChanged(bool);
    void rhinoBeetleQuantityChanged(int);
    void ladybugChanged(bool);
    void ladybugQuantityChanged(int);
    void sandCicadaChanged(bool);
    void sandCicadaQuantityChanged(int);
    void stagBeetleChanged(bool);
    void stagBeetleQuantityChanged(int);
    void grasshopperChanged(bool);
    void grasshopperQuantityChanged(int);
    void mantisChanged(bool);
    void mantisQuantityChanged(int);
    void antChanged(bool);
    void antQuantityChanged(int);
    void eldinRollerChanged(bool);
    void eldinRollerQuantityChanged(int);
    // Materials
    void hornetLarvaeChanged(bool);
    void hornetLarvaeQuantityChanged(int);
    void birdFeatherChanged(bool);
    void birdFeatherQuantityChanged(int);
    void tumbleWeedChanged(bool);
    void tumbleWeedQuantityChanged(int);
    void lizardTailChanged(bool);
    void lizardTailQuantityChanged(int);
    void eldinOreChanged(bool);
    void eldinOreQuantityChanged(int);
    void ancientFlowerChanged(bool);
    void ancientFlowerQuantityChanged(int);
    void amberRelicChanged(bool);
    void amberRelicQuantityChanged(int);
    void duskRelicChanged(bool);
    void duskRelicQuantityChanged(int);
    void jellyBlobChanged(bool);
    void jellyBlobQuantityChanged(int);
    void monsterClawChanged(bool);
    void monsterClawQuantityChanged(int);
    void monsterHornChanged(bool);
    void monsterHornQuantityChanged(int);
    void decoSkullChanged(bool);
    void decoSkullQuantityChanged(int);
    void evilCrystalChanged(bool);
    void evilCrystalQuantityChanged(int);
    void blueBirdFeatherChanged(bool);
    void blueBirdFeatherQuantityChanged(int);
    void goldenSkullChanged(bool);
    void goldenSkullQuantityChanged(int);
    void goddessPlumeChanged(bool);
    void goddessPlumeQuantityChanged(int);
    void gratitudeCrystalAmountChanged(int);

private:
    quint32 quantity(bool isRight, int offset) const;
    void    setQuantity(bool isRight, int offset, quint32 val);
    uint    gameOffset() const;
    QString readNullTermString(int offset) const;
    void    writeDataFile(const QString& filepath, char* data, quint64 len);
    void    writeNullTermString(const QString& val, int offset);
    bool    flag(quint32 offset, quint32 flag) const;
    void    setFlag(quint32 offset, quint32 flag, bool val);
    char*   m_data;
    QImage  m_bannerImage;
    QString m_filename;
    Game    m_game;
    bool    m_isOpen;
    bool    m_isDirty;
    WiiSave* m_saveGame;
    Checksum  m_checksumEngine;
};

#endif // GAMEFILE_H
