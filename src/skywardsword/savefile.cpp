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
#include "skywardsword/adventure.h"
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

SaveFile::SaveFile(const QString& filepath)
{
    open(filepath);
}

SaveFile::~SaveFile()
{
    m_adventures.clear();
}

QString SaveFile::gameName() const
{
    return "Skyward Sword";
}

bool SaveFile::open(const QString &filepath)
{
    if (!filepath.isEmpty())
    {
        QFile file;
        file.setFileName(filepath);
        if (file.exists() && file.open(QFile::ReadOnly))
        {
            m_filename = filepath;
            QByteArray data = file.readAll();
            m_region = (Region)qFromBigEndian(*(quint32*)(data.data()));
            if (data.size() == 0xFBE0)
            {
                for (int i = 0; i < 3; i++)
                {
                    Adventure adventure;
                    adventure.setData(data.mid(0x20 + (i*0x53C0), 0x53BC));
                    quint32 checksum = qFromBigEndian(*(quint32*)(data.data() + (0x20 + (i*0x53C0) + 0x53BC)));
                    adventure.setChecksum(checksum);
                    m_adventures.push_back(&adventure);
                }

                file.close();

                return true;
            }
            return false;
        }

        return false;
    }

    return false;
}

bool SaveFile::save(const QString &filepath)
{
    if (!filepath.isEmpty())
    {
        QFile file(filepath);
        if (file.open(QFile::WriteOnly))
        {
            QByteArray data(0x1D, 0);
            *(quint32*)(data.data()) = qToBigEndian((quint32)m_region);
            *(quint32*)(data.data() + 0x001C) = qToBigEndian(29); // Size of the header + 1;

            foreach(AdventureBase* adv, m_adventures)
            {
                data.append(adv->data());
                int checksum = qToBigEndian(adv->checksum());
                data.append((char*)&checksum, 4);
            }

            file.write(data);
            file.close();
            return true;
        }
        return false;
    }

    return false;
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
