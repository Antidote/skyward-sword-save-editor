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

#include "common.h"
#include <time.h>
#include <QtEndian>
#include <QDebug>

float swapFloat(float val)
{
#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
    float retVal;
    char* convFloat = (char*) &val;
    char* retFloat = (char*) &retVal;

    retFloat[0] = convFloat[3];
    retFloat[1] = convFloat[2];
    retFloat[2] = convFloat[1];
    retFloat[3] = convFloat[0];

    return retVal;
#else
    return val;
#endif
}

int convertRGB5A3ToBitMap(quint8* tplbuf, quint8** bitmapdata, quint32 width, quint32 height)
{
        quint32 x, y;
        quint32 x1, y1;
        quint32 iv;
        //tplpoint -= width;
        *bitmapdata = (quint8*)calloc(width * height, 4);
        if(*bitmapdata == NULL)
                return -1;
        quint32 outsz = width * height * 4;
        for(iv = 0, y1 = 0; y1 < height; y1 += 4) {
                for(x1 = 0; x1 < width; x1 += 4) {
                        for(y = y1; y < (y1 + 4); y++) {
                                for(x = x1; x < (x1 + 4); x++) {
                                        quint16 oldpixel = *(quint16*)(tplbuf + ((iv++) * 2));
                                        if((x >= width) || (y >= height))
                                                continue;
                                        oldpixel = qFromBigEndian(oldpixel);
                                        if(oldpixel & (1 << 15)) {
                                                // RGB5
                                                quint8 b = (((oldpixel >> 10) & 0x1F) * 255) / 31;
                                                quint8 g = (((oldpixel >> 5)  & 0x1F) * 255) / 31;
                                                quint8 r = (((oldpixel >> 0)  & 0x1F) * 255) / 31;
                                                quint8 a = 255;
                                                quint32 rgba = (r << 0) | (g << 8) | (b << 16) | (a << 24);
                                                (*(quint32**)bitmapdata)[x + (y * width)] = rgba;
                                        }else{
                                                // RGB4A3

                                            quint8 a = (((oldpixel >> 12) & 0x7) * 255) / 7;
                                            quint8 b = (((oldpixel >> 8)  & 0xF) * 255) / 15;
                                            quint8 g = (((oldpixel >> 4)  & 0xF) * 255) / 15;
                                            quint8 r = (((oldpixel >> 0)  & 0xF) * 255) / 15;
                                            quint32 rgba = (r << 0) | (g << 8) | (b << 16) | (a << 24);
                                            (*(quint32**)bitmapdata)[x + (y * width)] = rgba;
                                        }
                                }
                        }
                }
        }
        return outsz;
}

QImage convertTextureToImage( const QByteArray &ba, quint32 w, quint32 h )
{
    //qDebug() << "SaveBanner::ConvertTextureToImage" << ba.size() << hex << w << h;
    quint8* bitmapdata = NULL;//this will hold the converted image
    int ret = convertRGB5A3ToBitMap( (quint8*)ba.constData(), &bitmapdata, w, h );
    if( !ret )
    {
        qWarning() << "SaveBanner::ConvertTextureToImage -> error converting image";
        return QImage();
    }
    QImage im = QImage( (const uchar*)bitmapdata, w, h, QImage::Format_ARGB32 );
    QImage im2 = im.copy( im.rect() );//make a copy of the image so the "free" wont delete any data we still want
    free( bitmapdata );
    return im2;
}

quint64 getWiiTime()
{
    time_t sysTime, tzDiff, tzDST;
    struct tm * gmTime;

    time(&sysTime);

    // Account for DST where needed
    gmTime = localtime(&sysTime);
    if(gmTime->tm_isdst == 1)
        tzDST = 3600;
    else
        tzDST = 0;

    // Lazy way to get local time in sec
    gmTime	= gmtime(&sysTime);
    tzDiff = sysTime - mktime(gmTime);

    return (quint64)(TICKS_PER_SECOND * ((sysTime + tzDiff + tzDST) - SECONDS_TO_2000));
}

quint64 toWiiTime(QDateTime time)
{
    time_t sysTime, tzDiff, tzDST;
    struct tm * gmTime;

    sysTime = time.toTime_t();
    // Account for DST where needed
    gmTime = localtime(&sysTime);
    if(gmTime->tm_isdst == 1)
        tzDST = 3600;
    else
        tzDST = 0;

    // Lazy way to get local time in sec
    gmTime	= gmtime(&sysTime);
    tzDiff = sysTime - mktime(gmTime);

    return (quint64)(TICKS_PER_SECOND * ((sysTime + tzDiff + tzDST) - SECONDS_TO_2000));
}

QDateTime fromWiiTime(quint64 wiiTime)
{
    QDateTime tmp(QDate(2000, 1, 1));
    tmp = tmp.addSecs(wiiTime / TICKS_PER_SECOND);
    return tmp;
}



