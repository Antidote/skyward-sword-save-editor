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

#ifndef IGAMEFILE_H
#define IGAMEFILE_H

#include <QString>

class IGameFile : public QObject
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

    virtual bool isOpen() const=0;
    virtual void close() = 0;
    virtual bool reload(Game game) = 0;
    virtual bool save(const QString& filename = NULL) = 0;
    virtual bool open(Game game = GameNone, const QString& filepath=NULL) = 0;
    virtual void createNewGame(Game game) = 0;
    virtual void deleteGame(Game game = GameNone) = 0;
    virtual void deleteAllGames() = 0;
    virtual void updateChecksum() = 0;
    virtual bool hasValidChecksum() = 0;
    virtual bool isModified() const = 0;
    virtual QString filename() const = 0;
    virtual void setFilename(const QString& filepath) = 0;
};

#endif // IGAMEFILE_H
