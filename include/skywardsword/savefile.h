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
#include <QImage>
#include <QIcon>
#include "skywardsword/adventure.h"



class QDateTime;
class WiiSave;

namespace SkywardSword
{
//class Adventure;

class SaveFile : public SaveBase
{
public:
    enum Region
    {
         NTSCURegion = 0x45554F53,
         NTSCJRegion = 0x4A554F53,
         PALRegion   = 0x50554F53
    };

    SaveFile(Region region);
    SaveFile(const QString& filepath = NULL);
    ~SaveFile();

    QString gameName() const;
    bool    save(const QString& filepath = "");
    bool    open(const QString& filepath="");

    static bool isValidFile(const QString &filepath, Region* outRegion);

private:
    Region m_region;
};

} // SkywardSword
#endif // GAMEFILE_H
