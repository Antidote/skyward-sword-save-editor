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

#ifndef COMMON_H
#define COMMON_H

#include <QImage>
#include <QDateTime>

const quint64 SECONDS_TO_2000 = 946684800LL;
const quint64 TICKS_PER_SECOND = 60750000LL;

float swapFloat(float);
QImage ConvertTextureToImage( const QByteArray &ba, quint32 w, quint32 h );
quint64 getWiiTime();
quint64 toWiiTime(quint64 wiiTime);
QDateTime fromWiiTime(quint64 wiiTime);

#endif // COMMON_H
