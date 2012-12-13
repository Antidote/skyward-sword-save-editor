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

class IGameFile
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

    virtual ~IGameFile(){}

    virtual bool IsOpen() const=0;
    virtual void Close() = 0;
    virtual bool Reload(Game game) = 0;
    virtual bool Save(const QString& filename = NULL) = 0;
    virtual bool Open(Game game = GameNone, const QString& filepath=NULL) = 0;
    virtual void CreateNewGame(Game game) = 0;
    virtual void DeleteGame(Game game = GameNone) = 0;
    virtual void DeleteAllGames() = 0;
    virtual void UpdateChecksum() = 0;
    virtual bool HasValidChecksum() = 0;
    virtual bool IsModified() const = 0;

    virtual bool HasFileOnDiskChanged() = 0;

    virtual QString GetFilename() const = 0;
    virtual void SetFilename(const QString& filepath) = 0;
};

#endif // IGAMEFILE_H
