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

class SkywardSwordFile : public SaveBase
{
public:
    enum Region
    {
         NTSCURegion = 0x45554F53,
         NTSCJRegion = 0x4A554F53,
         PALRegion   = 0x50554F53
    };

    SkywardSwordFile(Region region);
    SkywardSwordFile(const QString& filepath = NULL, Game game = Game1);
    ~SkywardSwordFile();

    QString gameName() const;
    bool save(const QString& filepath = ""){}
    bool open(Game game = GameNone, const QString& filepath=""){}
    void createNewGame(Game game){}
    void createEmptyFile(Region region){}
    void deleteGame(Game game = GameNone){}
    void deleteAllGames(){}
    bool isModified() const{}

    void    close(){} //<! Closes the current file without saving.
    bool    reload(Game game){}
    bool    isOpen() const{}

    QString filename() const{}
    void    setFilename(const QString& filepath){}

    static bool isValidFile(const QString &filepath, Region* outRegion);

private:
};

#endif // GAMEFILE_H
