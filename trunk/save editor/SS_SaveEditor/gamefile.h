#ifndef GAMEFILE_H
#define GAMEFILE_H

#include <QFile>

class QDateTime;

struct PlayTime
{
    int Hours;
    int Mins;
    int Seconds;
    quint64 RawTicks;
};

class GameFile
{
public:
    enum Game
    {
        GameNone = -1,
        Game1,
        Game2,
        Game3
    };

    GameFile(const QString& filepath = NULL, Game game = Game1);
    ~GameFile();

    bool Save(const QString& filename = NULL);
    bool Open(Game game = GameNone, const QString& filepath=NULL);
    void Close(); //<! Closes the current file without saving.
    void Reload(Game game);

    bool IsOpen() const;

    QString GetFilename() const;
    void SetFilename(const QString& filepath);

    Game GetGame() const;
    void SetGame(Game game);

    uint GetGameOffset() const;

    PlayTime GetPlayTime() const;
    void  SetPlayTime(PlayTime val);
    QDateTime GetSaveTime() const;
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
    ushort GetRupees() const;
    void SetRupees(ushort val);
    ushort GetTotalHP() const;
    void SetTotalHP(ushort val);
    ushort GetUnkHP() const;
    void SetUnkHP(ushort val);
    ushort GetCurrentHP() const;
    void SetCurrentHP(ushort val);
    uint GetChecksum() const;
    QString GetCurrentMap() const;
    void SetCurrentMap(const QString& map);
    QString GetCurrentArea() const;
    void SetCurrentArea(const QString& map);
    QString GetCurrentRoom() const;
    void SetCurrentRoom(const QString& map);
    void UpdateChecksum();
    bool HasValidChecksum(); // for integrity checks

    bool IsModified() const;

    QString ReadNullTermString(int offset) const;
    void WriteNullTermString(const QString& val, int offset);

private:
    char* m_data;
    QString m_filename;
    Game m_game;
    bool m_isOpen;
    bool m_isDirty;
};

#endif // GAMEFILE_H
