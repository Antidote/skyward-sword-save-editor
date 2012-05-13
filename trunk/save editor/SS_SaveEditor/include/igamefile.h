#ifndef IGAMEFILE_H
#define IGAMEFILE_H

#include <QString>

class IGameFile
{
public:
    enum Game
    {
        GameNone = -1,
        Game1,
        Game2,
        Game3
    };

    virtual bool IsOpen() const=0;
    virtual void Close() = 0;
    virtual void Reload(Game game) = 0;
    virtual bool Save(const QString& filename = NULL) = 0;
    virtual bool Open(Game game = GameNone, const QString& filepath=NULL)=0;
    virtual void CreateNewGame(Game game) = 0;
    virtual void DeleteGame(Game game = GameNone);
    virtual void DeleteAllGames() = 0;
    virtual void UpdateChecksum() = 0;
    virtual bool HasValidChecksum() = 0;
    virtual bool IsModified() const = 0;

    virtual bool HasFileOnDiskChanged() = 0;

    virtual QString GetFilename() const = 0;
    virtual void SetFilename(const QString& filepath) = 0;
};

#endif // IGAMEFILE_H
