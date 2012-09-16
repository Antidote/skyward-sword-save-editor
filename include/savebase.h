#ifndef IGAMEFILE_H
#define IGAMEFILE_H

#include <QString>

class SaveBase
{
public:
    enum Game
    {
        GameNone = -1,
        Game1,
        Game2,
        Game3,
        GameCount
    };

    virtual ~SaveBase(){}

    virtual QString gameName() const=0;

    virtual bool isOpen() const=0;
    virtual void close() = 0;
    virtual bool reload(Game game) = 0;
    virtual bool save(const QString& filename = NULL) = 0;
    virtual bool open(Game game = GameNone, const QString& filepath=NULL) = 0;
    virtual void createNewGame(Game game) = 0;
    virtual void deleteGame(Game game = GameNone) = 0;
    virtual void deleteAllGames() = 0;
    virtual bool isModified() const = 0;

    virtual QString filename() const = 0;
    virtual void setFilename(const QString& filepath) = 0;
};

#endif // IGAMEFILE_H
