#ifndef ADVENTURE_H
#define ADVENTURE_H

#include "adventurebase.h"

#include <QFile>
#include "savebase.h"
#include <QImage>
#include <QIcon>
#include "WiiSave.h"
#include "WiiBanner.h"
#include "checksum.h"

class QDateTime;
class WiiSave;

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

class Adventure : AdventureBase
{
public:
    // Default coords for new file
    static const float DEFAULT_POS_X;
    static const float DEFAULT_POS_Y;
    static const float DEFAULT_POS_Z;
    static const ushort JAPANESE_NAME[4];

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

    Adventure();
    bool      isModified() const;

    PlayTime  playtime() const;
    void      setPlaytime(PlayTime val);
    QDateTime savetime() const;
    void      setSavetime();
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
    void      setPlayerName(const QString& name);
    bool      isHeroMode() const;
    void      setHeroMode(bool val);
    bool      introViewed() const;
    void      setIntroViewed(bool val);
    bool      sword(Sword sword) const;
    void      setSword(Sword sword, bool val);
    bool      equipment(WeaponEquipment weapon) const;
    void      setEquipment(WeaponEquipment, bool val);
    bool      bug(Bug bug) const;
    void      setBug(Bug bug, bool val);
    quint32   bugQuantity(Bug bug) const;
    void      setBugQuantity(Bug bug, quint32 val);
    bool      material(Material material);
    void      setMaterial(Material material, bool val);
    quint32   materialQuantity(Material material);
    void      setMaterialQuantity(Material material, quint32 val);
    quint32   gratitudeCrystalAmount();
    void      setGratitudeCrystalAmount(quint32 val);
    ushort    rupees() const;
    void      setRupees(ushort val);
    ushort    totalHP() const;
    void      setTotalHP(ushort val);
    ushort    unkHP() const;
    void      setUnkHP(ushort val);
    ushort    currentHP() const;
    void      setCurrentHP(ushort val);
    uint      roomID() const;
    void      setRoomID(uint val);
    QString   currentMap() const;
    void      setCurrentMap(const QString& map);
    QString   currentArea() const;
    void      setCurrentArea(const QString& map);
    QString   currentRoom() const;
    void      setCurrentRoom(const QString& map);
    void      setGameData(const QByteArray &data);
    bool      isNew() const;
    void      setNew(bool val);
    uint      checksum() const;
    void      updateChecksum();

    QByteArray gameData();

private:
    quint32 quantity(bool isRight, int offset) const;
    void    setQuantity(bool isRight, int offset, quint32 val);
    QString readNullTermString(int offset) const;
    void    writeNullTermString(const QString& val, int offset);
    bool    flag(quint32 offset, quint32 flag) const;
    void    setFlag(quint32 offset, quint32 flag, bool val);

    quint32 m_checksum;
    Checksum m_checksumEngine;
};

#endif // ADVENTURE_H
