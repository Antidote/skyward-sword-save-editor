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

#include "skywardsword/savefile.h"
#include "common.h"
#include "WiiSave.h"
#include "WiiBanner.h"
#include "WiiFile.h"
#include "Exception.hpp"
#include "wiikeys.h"
#include "checksum.h"
#include <QMessageBox>

#include <QtEndian>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <time.h>

namespace SkywardSword
{

// This constructor allows us to create a new save file.
SaveFile::SaveFile(Region region)
{
    m_region = region;
}

SaveFile::SaveFile(const QString& filepath, Game game)
{
    open(game, filepath);
}

SaveFile::~SaveFile()
{
}

QString SaveFile::gameName() const
{
    return "Skyward Sword";
}

bool SaveFile::isValidFile(const QString &filepath, Region* outRegion)
{

    FILE* file = fopen(filepath.toAscii(), "rb");
    if (!file)
        return false;

    Region region;
    fread(&region, 4, 1, file);
    fseek(file, 0, SEEK_END);
    quint32 size = ftell(file);
    fclose(file);
    *outRegion = region;
    return (region == NTSCURegion || region == NTSCJRegion || region == PALRegion) && size == 0xFBE0;
}

} // SkywardSword
