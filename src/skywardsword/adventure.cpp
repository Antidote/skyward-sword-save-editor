#include "skywardsword/adventure.h"
#include "qhexedit2/qhexedit.h"
#include "common.h"
#include <time.h>
#include <QDateTime>
#include <QtEndian>

namespace SkywardSword
{

Adventure::Adventure()
{
}


PlayTime Adventure::playtime() const
{
    PlayTime playTime;
    quint64 tmp = qFromBigEndian<quint64>(*(quint64*)(m_hexData->data().data()));
    playTime.Hours = ((tmp / TICKS_PER_SECOND) / 60) / 60;
    playTime.Minutes =  ((tmp / TICKS_PER_SECOND) / 60) % 60;
    playTime.Seconds = ((tmp / TICKS_PER_SECOND) % 60);
    playTime.RawTicks = tmp;
    return playTime;
}

// Sets the current playtime
void Adventure::setPlaytime(PlayTime val)
{
    quint64 totalSeconds = (val.Hours * 60) * 60;
    totalSeconds += val.Minutes * 60;
    totalSeconds += val.Seconds;
    totalSeconds *= TICKS_PER_SECOND;
    m_hexData->replace(0, 8, QByteArray((char*)qToBigEndian(totalSeconds), 8));
}

QDateTime Adventure::savetime() const
{
    QDateTime tmp(QDate(2000, 1, 1));
    tmp = tmp.addSecs(qFromBigEndian((quint64)(*(quint64*)(m_hexData->data().data() + 0x0008)) / 60750546));
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

void Adventure::setSavetime()
{
    m_hexData->replace(0x0008, 8, QByteArray((char*)(quint64)qToBigEndian((quint64)((GetLocalTimeSinceJan1970() - SECONDS_TO_2000) * 60749453.59)), 8));
}

Vector3 Adventure::playerPosition() const
{
    return Vector3(swapFloat(*(float*)(m_hexData->data().data() + 0x0010)),
                   swapFloat(*(float*)(m_hexData->data().data() + 0x0014)),
                   swapFloat(*(float*)(m_hexData->data().data() + 0x0018)));
}

void Adventure::setPlayerPosition(float x, float y, float z)
{
    setPlayerPosition(Vector3(x, y, z));
}

void Adventure::setPlayerPosition(Vector3 pos)
{
    pos.X = swapFloat(pos.X);
    pos.Y = swapFloat(pos.Y);
    pos.Z = swapFloat(pos.Z);
    m_hexData->replace(0x0010, 4, QByteArray((char*)&pos.X, 4));
    m_hexData->replace(0x0014, 4, QByteArray((char*)&pos.Y, 4));
    m_hexData->replace(0x0018, 4, QByteArray((char*)&pos.Z, 4));
}

Vector3 Adventure::playerRotation() const
{
    return Vector3(swapFloat(*(float*)(m_hexData->data().data() + 0x001C)),
                   swapFloat(*(float*)(m_hexData->data().data() + 0x0020)),
                   swapFloat(*(float*)(m_hexData->data().data() + 0x0024)));
}

void Adventure::setPlayerRotation(float roll, float pitch, float yaw)
{
    setPlayerRotation(Vector3(roll, pitch, yaw));
}

void Adventure::setPlayerRotation(Vector3 rotation)
{
    rotation.X = swapFloat(rotation.X);
    rotation.Y = swapFloat(rotation.Y);
    rotation.Z = swapFloat(rotation.Z);
    m_hexData->replace(0x001C, 4, QByteArray((char*)&rotation.X, 4));
    m_hexData->replace(0x0020, 4, QByteArray((char*)&rotation.Y, 4));
    m_hexData->replace(0x0024, 4, QByteArray((char*)&rotation.Z, 4));
}

Vector3 Adventure::cameraPosition() const
{
    return Vector3(swapFloat(*(float*)(m_hexData->data().data() + 0x0028)),
                   swapFloat(*(float*)(m_hexData->data().data() + 0x002C)),
                   swapFloat(*(float*)(m_hexData->data().data() + 0x0030)));
}

void Adventure::setCameraPosition(float x, float y, float z)
{
    setCameraPosition(Vector3(x, y, z));
}

void Adventure::setCameraPosition(Vector3 pos)
{
    pos.X = swapFloat(pos.X);
    pos.Y = swapFloat(pos.Y);
    pos.Z = swapFloat(pos.Z);
    m_hexData->replace(0x0028, 4, QByteArray((char*)&pos.X, 4));
    m_hexData->replace(0x002C, 4, QByteArray((char*)&pos.Y, 4));
    m_hexData->replace(0x0030, 4, QByteArray((char*)&pos.Z, 4));
}

Vector3 Adventure::cameraRotation() const
{
    return Vector3(swapFloat(*(float*)(0x003C + 0x0034)),
                   swapFloat(*(float*)(m_hexData->data().data() + 0x0038)),
                   swapFloat(*(float*)(m_hexData->data().data() + 0x003C)));
}

void Adventure::setCameraRotation(float roll, float pitch, float yaw)
{
    setCameraRotation(Vector3(roll, pitch, yaw));
}

void Adventure::setCameraRotation(Vector3 rotation)
{
    rotation.X = swapFloat(rotation.X);
    rotation.Y = swapFloat(rotation.Y);
    rotation.Z = swapFloat(rotation.Z);
    m_hexData->replace(0x0034, 4, QByteArray((char*)&rotation.X, 4));
    m_hexData->replace(0x0038, 4, QByteArray((char*)&rotation.Y, 4));
    m_hexData->replace(0x003C, 4, QByteArray((char*)&rotation.Z, 4));
}

QString Adventure::playerName() const
{
    ushort tmpName[8];
    for (int i = 0, j=0; i < 8; ++i, j+= 2)
    {
        tmpName[i] = *(ushort*)(m_hexData->data().data() + (0x08D4 + j));
        tmpName[i] = qFromBigEndian<quint16>(tmpName[i]);
    }

    return QString(QString::fromUtf16(tmpName));
}

void Adventure::setPlayerName(const QString &name)
{
    QByteArray nameArray;
    for (int i = 0, j = 0; i < 8; ++i, ++j)
    {
        if (i > name.length())
        {
            nameArray.push_back((char*)(ushort)0);
            continue;
        }

        ushort tmp = qToBigEndian(name.utf16()[i]);
        nameArray.push_back((char*)&tmp);
    }

   m_hexData->replace(0x08D4, 8*2, nameArray);
}

bool Adventure::isHeroMode() const
{

    return flag(0x08FE, 0x08);
}

void Adventure::setHeroMode(bool val)
{
    setFlag(0x08FE, 0x08, val);
}

bool Adventure::introViewed() const
{

    return *(char*)(m_hexData->data().data() + 0x0941) != 0;
}

void Adventure::setIntroViewed(bool val)
{
    if (val)
        m_hexData->replace(0x0941, 1, (char*)0x2);
    else
        m_hexData->replace(0x0941, 1, (char*)0x0);
}


bool Adventure::sword(Sword sword) const
{
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

void Adventure::setSword(Sword sword, bool val)
{
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
}

bool Adventure::equipment(WeaponEquipment weapon) const
{
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
            return flag(0x09E4,0x10);
        case IronBowWeapon:
            return flag(0x09ED, 0x01);
        case SacredBowWeapon:
            return flag(0x09ED, 0x02);
        case HarpEquipment:
            return flag(0x09F4, 0x02);
        default:
            return false;
    }
}

void Adventure::setEquipment(WeaponEquipment weapon, bool val)
{
    switch(weapon)
    {
        case SlingshotWeapon:   setFlag(0x09E6, 0x10, val); break;
        case ScattershotWeapon: setFlag(0x09EC, 0x80, val); break;
        case BugnetWeapon:      setFlag(0x09E8, 0x01, val); break;
        case BigBugnetWeapon:   setFlag(0x09F2, 0x02, val); break;
        case BeetleWeapon:      setFlag(0x09E6, 0x20, val); break;
        case HookBeetleWeapon:  setFlag(0x09EB, 0x02, val); break;
        case QuickBeetleWeapon: setFlag(0x09EB, 0x04, val); break;
        case ToughBeetleWeapon: setFlag(0x09EB, 0x08, val); break;
        case BombWeapon:        setFlag(0x09ED, 0x04, val); break;
        case GustBellowsWeapon: setFlag(0x09E6, 0x02, val); break;
        case WhipWeapon:        setFlag(0x09F3, 0x10, val); break;
        case ClawshotWeapon:    setFlag(0x09E4, 0x20, val); break;
        case BowWeapon:         setFlag(0x09E4, 0x10, val); break;
        case IronBowWeapon:     setFlag(0x09ED, 0x01, val); break;
        case SacredBowWeapon:   setFlag(0x09ED, 0x02, val); break;
        case HarpEquipment:     setFlag(0x09F4, 0x02, val); break;
        default: return;
    }
}

bool Adventure::bug(Bug bug) const
{
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

void Adventure::setBug(Bug bug, bool val)
{
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
}

quint32 Adventure::bugQuantity(Bug bug) const
{
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

void Adventure::setBugQuantity(Bug bug, quint32 val)
{
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
}

bool Adventure::material(Material material)
{
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

void Adventure::setMaterial(Material material, bool val)
{
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
}

quint32 Adventure::materialQuantity(Material material)
{
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

void Adventure::setMaterialQuantity(Material material, quint32 val)
{
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
}

quint32 Adventure::gratitudeCrystalAmount()
{
    quint32 ret = (quint32)((qFromBigEndian<quint16>(*(quint16*)(m_hexData->data().data() + 0x0A50)) >> 3) & 127);
    return ret;
}

void Adventure::setGratitudeCrystalAmount(quint32 val)
{
    val = (quint16)(val << 3);
    val = qToBigEndian((quint16)val);
    m_hexData->replace(0x0A50, 2, QByteArray((char*)&val, 2));
}

ushort Adventure::rupees() const
{
    return qFromBigEndian(*(ushort*)(m_hexData->data().data() + 0x0A5E));
}

void Adventure::setRupees(ushort val)
{
    val = qToBigEndian(val);
    m_hexData->replace(0x0A5E, 2, QByteArray((char*)&val, 2));
}

ushort Adventure::totalHP() const
{
    return qToBigEndian(*(ushort*)(m_hexData->data().data() + 0x5302));
}

void Adventure::setTotalHP(ushort val)
{
    val = qToBigEndian(val);
    m_hexData->replace(0x5302, 2, QByteArray((char*)&val));
}

ushort Adventure::unkHP() const
{

    return qFromBigEndian(*(ushort*)(m_hexData->data().data() + 0x5304));
}

void Adventure::setUnkHP(ushort val)
{
    val = qToBigEndian(val);
    m_hexData->replace(0x5304, 2, QByteArray((char*)&val, 2));
}

ushort Adventure::currentHP() const
{
    return qFromBigEndian<quint16>(*(quint16*)(m_hexData->data().data() + 0x5306));
}

void Adventure::setCurrentHP(ushort val)
{
    val = qToBigEndian(val);
    m_hexData->replace(0x5306, 2, QByteArray((char*)&val));
}

uint Adventure::roomID() const
{
    return (uint)(*(uchar*)(m_hexData->data().data() + 0x5309));
}

void Adventure::setRoomID(uint val)
{
    val = qToBigEndian(val);
    m_hexData->replace(0x5309, 2, QByteArray(1, (uchar)val));
}

QString Adventure::currentMap() const
{
    return readNullTermString(0x531c);
}

void Adventure::setCurrentMap(const QString& map)
{
    writeNullTermString(map, 0x531c);
}

QString Adventure::currentArea() const
{
    return readNullTermString(0x533c);
}

void Adventure::setCurrentArea(const QString& map)
{
    writeNullTermString(map, 0x533c);
}

QString Adventure::currentRoom() const // Not sure about this one
{
    return readNullTermString(0x535c);
}

void Adventure::setCurrentRoom(const QString& map) // Not sure about this one
{
    writeNullTermString(map, 0x535c);
}

void Adventure::setGameData(const QByteArray &data)
{
    if (data.length() == 0x53C0)
        m_hexData->setData(data);
}

QByteArray Adventure::gameData()
{
    return m_hexData->data();
}

uint Adventure::checksum() const
{
    return m_checksum;
}

void Adventure::updateChecksum()
{
    m_checksum = qToBigEndian(m_checksumEngine.GetCRC32((quint8*)m_hexData->data().data(), 0, 0x53BC)); // change it to Big Endian
}

bool Adventure::isNew() const
{
    return (*(char*)(m_hexData->data().data() + 0x53AD)) != 0;
}

void Adventure::setNew(bool val)
{
    val = qToBigEndian((quint32)val);
    m_hexData->replace(0x53AD, 2, QByteArray((char*)&val, 2));
}

bool Adventure::isModified() const
{
    return !m_hexData->undoStack()->index() == m_hexData->undoStack()->cleanIndex();
}

QString Adventure::readNullTermString(int offset) const
{
    QString ret("");
    char c = m_hexData->data().data()[offset];
    while (c != '\0')
    {
        ret.append(c);
        c = m_hexData->data().data()[offset];
    }

    return ret;
}

void Adventure::writeNullTermString(const QString& val, int offset)
{
    char c = val.toStdString().c_str()[0];
    QByteArray tmp;
    int i = 0;
    while (c != '\0')
    {
        tmp.push_back(&c);
        c = val.toStdString().c_str()[++i];

    }
    tmp.push_back('\0');
    m_hexData->replace(offset, tmp.length(), tmp);
}

bool Adventure::flag(quint32 offset, quint32 flag) const
{
    return (*(char*)(m_hexData->data().data() + offset) & flag) == flag;
}

void Adventure::setFlag(quint32 offset, quint32 flag, bool val)
{
    char curVal = *(char*)(m_hexData->data().data() + offset);
    if (val)
        curVal |= flag;
    else
        curVal &= ~flag;

    m_hexData->replace(offset, 1, QByteArray(1, curVal));
}
quint32 Adventure::quantity(bool isRight, int offset) const
{
    switch(isRight)
    {
        case false:
            return (quint32)(qFromBigEndian((*(quint16*)(m_hexData->data().data() + offset))) >> 7) & 127;
        case true:
            return (quint32)(qFromBigEndian(*(quint16*)(m_hexData->data().data() + offset))) & 127;
    }

    return 0;
}

void Adventure::setQuantity(bool isRight, int offset, quint32 val)
{
    quint16 oldVal = qFromBigEndian(*(quint16*)(m_hexData->data().data() + offset));
    quint16 newVal = 0;
    switch(isRight)
    {
        case false:
            newVal = (oldVal&127)|(((quint16)val << 7));
        break;
        case true:
            newVal = (oldVal >> 7) & 127;
        break;
    }

    newVal = qToBigEndian((quint16)newVal);
    m_hexData->replace(offset, 2, QByteArray((char*)&newVal, 2));
}

// To support MSVC I have placed these here, why can't Microsoft follow real ANSI Standards? <.<
const float Adventure::DEFAULT_POS_X = -4798.150391f;
const float Adventure::DEFAULT_POS_Y =  1237.629517f;
const float Adventure::DEFAULT_POS_Z = -6573.722656f;
const ushort Adventure::JAPANESE_NAME[4] = {0x30ea, 0x30f3, 0x30af, 0};

} // SkywardSword
