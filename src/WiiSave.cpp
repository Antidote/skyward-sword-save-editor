#include "WiiSave.h"
#include "WiiFile.h"
#include "WiiBanner.h"
#include "BinaryReader.hpp"
#include "BinaryWriter.hpp"
#include "IOException.hpp"
#include "aes.h"
#include "ec.h"
#include "tools.h"
#include "utility.h"
#include "md5.h"
#include "utf8.h"
#include "sha1.h"

#include <stdio.h>
#include <vector>
#include <string.h>
#include <sys/stat.h>
#include <iostream>
#include <iomanip>

const u8 sd_key[16]      = {0xab, 0x01, 0xb9, 0xd8, 0xe1, 0x62, 0x2b, 0x08, 0xaf, 0xba, 0xd8, 0x4d, 0xbf, 0xc2, 0xa5, 0x5d};
const u8 sd_iv[16]       = {0x21, 0x67, 0x12, 0xe6, 0xaa, 0x1f, 0x68, 0x9f, 0x95, 0xc5, 0xa2, 0x23, 0x24, 0xdc, 0x6a, 0x98};
const u8 md5_blanker[16] = {0x0e, 0x65, 0x37, 0x81, 0x99, 0xbe, 0x45, 0x17, 0xab, 0x06, 0xec, 0x22, 0x45, 0x1a, 0x57, 0x93};

WiiSave::WiiSave() :
    m_banner(NULL),
    m_reader(NULL),
    m_writer(NULL)
{
}

WiiSave::WiiSave(const std::string& filename) :
    m_banner(NULL),
    m_reader(NULL),
    m_writer(NULL)
{
    if (!loadFromFile(filename))
        throw std::string("Error loading file");
}

WiiSave::~WiiSave()
{
    m_files.clear();

    if (m_banner)
        delete m_banner;
    m_banner = NULL;

    if(m_reader)
        delete m_reader;
    if (m_writer)
        delete m_writer;
    m_reader = NULL;
    m_writer = NULL;

    std::cout << "Done." << std::endl;
}

bool WiiSave::loadFromFile(const std::string& filename)
{
    FILE* f = fopen(filename.c_str(), "rb");
    u8* data;
    int length;

    if (!f)
    {
        std::cerr << "Couldn't open file \"" << filename << "\" please check that it exists." << std::endl;
        return false;
    }

    fseek(f, 0, SEEK_END);
    length = ftell(f);
    fseek(f, 0, SEEK_SET);

    data = new u8[length];
    fread(data, 1, length, f);
    return loadFromMemory(data, length);
}

bool WiiSave::loadFromMemory(const u8* data, u64 length)
{
    if (m_reader == NULL)
        m_reader = new BinaryReader(data, length);
    else
    {
        m_reader->setData(data, length);
    }

    m_reader->setEndianess(Stream::BigEndian);

    m_banner = readBanner();
    if (!m_banner)
    {
        std::cerr << "Invalid banner" << std::endl;
        return false;
    }

    int bkVer = m_reader->readUInt32();
    bkVer = bkVer;
    if (bkVer != 0x00000070)
    {
        std::cerr << "Invalid BacKup header size: 0x" << std::hex << bkVer << std::endl;
        return false;
    }

    int bkMagic = m_reader->readUInt32();
    bkMagic = bkMagic;
    if (bkMagic != 0x426B0001)
    {
        std::cerr << "Invalid BacKup header magic: 0x" << std::hex << bkMagic << std::endl;
        return false;
    }

    int ngId = m_reader->readUInt32();
    ngId = ngId;

    int numFiles = m_reader->readUInt32();

    /*int fileSize =*/ m_reader->readUInt32();
    m_reader->seek(8); // skip unknown data;

    int totalSize = m_reader->readUInt32();
    m_reader->seek(64); // Unknown (Most likely padding)
    m_reader->seek(8);
    m_reader->seek(6);
    m_reader->seek(2);
    m_reader->seek(0x10);

    WiiFile* file;
    for (int i = 0; i < numFiles; ++i)
    {
        file = readFile();
        if (file)
            addFile("/" + file->filename(), file);
    }

    readCerts(totalSize);
    return true;
}

bool WiiSave::saveToFile(const std::string& filepath, u8* macAddress, int ngId, u8* ngPriv, u8* ngSig, int ngKeyId)
{
    m_writer = new BinaryWriter(filepath);
    m_writer->setAutoResizing(true);
    m_writer->setEndianess(Stream::BigEndian);

    writeBanner();

    m_writer->writeInt32(0x70);
    m_writer->writeInt32(0x426B0001);
    m_writer->writeInt32(ngId); // NG-ID
    m_writer->writeInt32(m_files.size());
    m_writer->writeInt32(0); // Size of files;
    m_writer->seek(8);
    m_writer->writeInt32(0); // totalSize
    m_writer->seek(64);
    m_writer->writeInt64(m_banner->gameID());
    m_writer->writeBytes((Int8*)macAddress, 6);
    m_writer->seek(2); // unknown;
    m_writer->seek(0x10); // padding;
    int totalSize = 0;
    for (std::map<std::string, WiiFile*>::const_iterator iter = m_files.begin(); iter != m_files.end(); ++iter)
    {
        totalSize += writeFile(iter->second);
    }
    int pos = m_writer->position();
    // Write size data
    m_writer->seek(0xF0C0 + 0x10, Stream::Beginning);
    m_writer->writeInt32(totalSize);
    m_writer->seek(0xF0C0 + 0x1C, Stream::Beginning);
    m_writer->writeInt32(totalSize + 0x3c0);
    m_writer->seek(pos, Stream::Beginning);

    writeCerts(totalSize, ngId, ngPriv, ngSig, ngKeyId);

    m_writer->Save();

    return true;
}

void WiiSave::addFile(const std::string& filepath, WiiFile* file)
{
    m_files[filepath] = file;
}

WiiFile* WiiSave::getFile(const std::string& filepath) const
{
    std::map<std::string, WiiFile*>::const_iterator iter = m_files.begin();

    for (;iter != m_files.end(); ++iter)
    {
        if (iter->first == filepath) // why does this work and not iter->first == filepath? :/
            return (WiiFile*)iter->second;
    }

    return NULL;
}

std::map<std::string, WiiFile*>& WiiSave::getFileList()
{
    return m_files;
}

void WiiSave::setBanner(WiiBanner* banner)
{
    m_banner = banner;
}

WiiBanner* WiiSave::banner() const
{
    return m_banner;
}

WiiBanner* WiiSave::readBanner()
{
    u8* dec = new u8[0xf0c0];
    memset(dec, 0, 0xF0C0);
    u8* data = (u8*)m_reader->readBytes(0xF0C0);
    u8* oldData = m_reader->data();
    u64 oldPos = m_reader->position();
    u64 oldLen = m_reader->length();
    u64 gameId;
    u32 bannerSize;
    u8  permissions;
    u8  md5[16];
    u8  md5Calc[16];
    u8  tmpIV[26];
    memcpy(tmpIV, sd_iv, 16);

    aes_set_key(sd_key);
    aes_decrypt(tmpIV, data, dec, 0xF0C0);
    memset(md5, 0, 16);
    memset(md5Calc, 0, 16);
    // Read in the MD5 sum
    memcpy(md5, (dec + 0x0E), 0x10);
    // Write the blanker to the buffer
    memcpy((dec + 0x0E), md5_blanker, 0x10);
    MD5(md5Calc, dec, 0xF0C0);

    // Compare the Calculated MD5 to the one from the file.
    // This needs to be done incase the file is corrupted.
    if (memcmp(md5, md5Calc, 0x10))
    {
        std::cerr << "MD5 Mismatch" << std::endl;
        // Make sure to reset m_reader values back to the old ones.
        std::cerr << "MD5 provided:   ";
        for (int i = 0; i < 16; ++i)
            std::cerr << std::hex << (int)(md5[i]);
        std::cerr << std::endl;

        std::cerr << "MD5 Calculated: ";
        for (int i = 0; i < 16; ++i)
            std::cerr << std::hex << (int)(md5Calc[i]);
        std::cerr << std::endl;
        m_reader->setData(oldData, oldLen);
        m_reader->seek(oldPos, Stream::Beginning);
        return NULL;
    }
    // Set the binary reader buffer;
    m_reader->setData(dec, 0xF0C0);
    // Start reading the header
    gameId = m_reader->readUInt64();
    bannerSize = m_reader->readUInt32();
    permissions = m_reader->readByte();
/*    unk =*/ m_reader->readByte();
    m_reader->seek(0x10);
    // skip padding
    m_reader->seek(2);

    int magic;
    int flags;
    short animSpeed;
    std::string gameTitle;
    std::string subTitle;

    magic = m_reader->readUInt32();

    // Ensure that the header magic is valid.
    if (magic != 0x5749424E)
    {
        std::cerr << "Invalid Header Magic: 0x" << std::hex << magic << std::endl;
        // Make sure to reset m_reader values back to the old ones.
        m_reader->setData(oldData, oldLen);
        m_reader->seek(oldPos, Stream::Beginning);
        return NULL;
    }

    flags = m_reader->readUInt32();
    animSpeed = m_reader->readUInt16();
    m_reader->seek(22);

    std::vector<short> tmp;
    for (int i = 0; i < 32; ++i)
    {
        short chr = m_reader->readUInt16();
        if (chr)
            tmp.push_back(chr);
        else
        {
            tmp.push_back((short)0);
            break;
        }
    }
    if (m_reader->position() != 0x0080)
        m_reader->seek(0x0080, Stream::Beginning);

    utf8::utf16to8(tmp.begin(), tmp.end(), back_inserter(gameTitle));

    tmp.clear();
    for (int i = 0; i < 32; ++i)
    {
        short chr = m_reader->readUInt16();
        if (chr)
            tmp.push_back(chr);
        else
        {
            tmp.push_back((short)0);
            break;
        }
    }
    if (m_reader->position() != 0x00C0)
        m_reader->seek(0x00C0, Stream::Beginning);
    utf8::utf16to8(tmp.begin(), tmp.end(), back_inserter(subTitle));

    WiiBanner* banner = new WiiBanner;
    banner->setGameID(gameId);
    banner->setTitle(gameTitle);
    banner->setSubtitle(subTitle);
    banner->setBannerSize(bannerSize);
    std::cerr << "Banner Size: " << std::hex << bannerSize << std::endl;
    WiiImage* bannerImage = readImage(192, 64);
    banner->setBannerImage(bannerImage);
    banner->setAnimationSpeed(animSpeed);
    banner->setPermissions(permissions);
    banner->setFlags(flags);


    if (banner->bannerSize() == 0x72a0)
    {
        WiiImage* icon = readImage(48, 48);
        if (icon)
            banner->addIcon(icon);
        else
            std::cerr << "Warning: Icon empty, skipping" << std::endl;
    }
    else
    {
        for(int i = 0; i < 8; i++)
        {
            WiiImage* icon = readImage(48, 48);
            if (icon)
                banner->addIcon(icon);
            else
                std::cerr << "Warning: Icon empty, skipping" << std::endl;
        }
    }

    m_reader->setData(oldData, oldLen);
    m_reader->seek(oldPos, Stream::Beginning);
    return banner;
}

WiiImage* WiiSave::readImage(int width, int height)
{
    u8* image = (u8*)m_reader->readBytes(width*height*2);

    if (!isEmpty(image, width*height*2))
        return new WiiImage(width, height, image);

    return NULL;
}

WiiFile* WiiSave::readFile()
{
    int fileLen;
    u8 permissions;
    u8 attributes;
    u8 type;
    std::string name;
    u8* filedata;
    WiiFile* ret;

    int magic = m_reader->readUInt32();
    if (magic != 0x03adf17e)
    {
        std::cerr << "Not a valid File entry header: 0x" << std::hex << magic << std::endl;
        return NULL;
    }

    fileLen = m_reader->readUInt32();
    permissions = m_reader->readByte();
    attributes  = m_reader->readByte();
    type        = (WiiFile::Type)m_reader->readByte();
    name        = std::string((const char*)m_reader->readBytes(0x45));
    ret = new WiiFile(std::string(name));
    ret->setPermissions(permissions);
    ret->setAttributes(attributes);
    ret->setType((WiiFile::Type)type);
    u8* iv = (u8*)m_reader->readBytes(0x10);
    m_reader->seek(0x20);

    if (type == WiiFile::File)
    {
        // Read file data
        int roundedLen = (fileLen + 63) & ~63;
        filedata = (u8*)m_reader->readBytes(roundedLen);

        // Decrypt file
        u8* decData = new u8[roundedLen];
        aes_set_key(sd_key);
        aes_decrypt(iv, filedata, decData, roundedLen);
        delete filedata;
        ret->setData(decData);
        ret->setLength(fileLen);
    }

    return ret;
}

void WiiSave::readCerts(u32 totalSize)
{
    u32 dataSize = totalSize - 0x340;
    u8* sig    = (u8*)m_reader->readBytes(0x40);
    u8* ngCert = (u8*)m_reader->readBytes(0x180);
    u8* apCert = (u8*)m_reader->readBytes(0x180);
    m_reader->seek(0xF0C0, Stream::Beginning);
    u8* data   = (u8*)m_reader->readBytes(dataSize);
    u8* hash;

    hash = getSha1(data, dataSize);
    u8* hash2 = getSha1(hash, 20);

    int ok = check_ec(ngCert, apCert, sig, hash2);

    //if (!ok)
        //throw IOException("Certificates failed validation!");
    std::cout << "Ok: " << ok << std::endl;
}

void WiiSave::writeBanner()
{
    m_writer->setEndianess(Stream::BigEndian);
    m_writer->setAutoResizing(true);
    m_writer->writeInt64(m_banner->gameID());
    m_writer->writeInt32((0x60a0+0x1200)*m_banner->icons().size());
    m_writer->writeByte((Int8)m_banner->permissions());
    m_writer->seek(1);
    m_writer->writeBytes((Int8*)md5_blanker, 16);
    m_writer->seek(2);
    m_writer->writeInt32(0x5749424E); // WIBN
    m_writer->writeInt32(m_banner->flags());
    m_writer->writeInt16(m_banner->animationSpeed());
    m_writer->seek(22);

    std::string str = "\xEF\xBB\xBF" + m_banner->title();

    std::vector<short> tmp;

    utf8::utf8to16(str.begin(), str.end(), back_inserter(tmp));

    for (int i = 0; i <= 32; ++i)
    {
        if (i >= tmp.size())
            break;
        u16 chr = tmp[i];
        if (chr != 0xFEFF)
            m_writer->writeInt16(chr);
    }

    if (m_writer->position() != 0x0080)
        m_writer->seek(0x0080, Stream::Beginning);
    str = "\xEF\xBB\xBF" + m_banner->subtitle();
    tmp.clear();

    utf8::utf8to16(str.begin(), str.end(), back_inserter(tmp));

    for (u32 i = 0; i < tmp.size(); ++i)
    {
        if (i >= tmp.size())
            break;

        u16 chr = tmp[i];
        if (chr != 0xFEFF)
            m_writer->writeInt16(chr);
    }

    if (m_writer->position() != 0x00C0)
        m_writer->seek(0x00C0, Stream::Beginning);

    WiiImage* bannerImage = m_banner->bannerImage();
    m_writer->writeBytes((Int8*)bannerImage->data(), bannerImage->width()*bannerImage->height()*2);

    // For empty icons
    u8* tmpIcon = new u8[48*48*2];
    memset(tmpIcon, 0, 48*48*2);
    for (u32 i = 0; i < 8; ++i)
    {
        if (i < m_banner->icons().size())
        {
            writeImage(m_banner->icons()[i]);
        }
        else
        {
            m_writer->writeBytes((Int8*)tmpIcon, 48*48*2);
        }
    }
    m_writer->Save();
    delete[] tmpIcon; // delete tmp buffer;

    u8* hash = new u8[0x10];
    MD5(hash, (u8*)m_writer->data(), 0xF0C0);
    m_writer->seek(0x0E, Stream::Beginning);
    m_writer->writeBytes((Int8*)hash, 0x10);

    aes_set_key(sd_key);
    u8 data[0xF0C0];
    memcpy(data, m_writer->data(), 0xF0C0);
    u8  tmpIV[26];
    memcpy(tmpIV, sd_iv, 16);
    aes_encrypt(tmpIV, data, data, 0xF0C0);

    m_writer->seek(0, Stream::Beginning);
    m_writer->writeBytes((Int8*)data, 0xF0C0);
    m_writer->seek(0xF0C0, Stream::Beginning);
}


void WiiSave::writeImage(WiiImage* image)
{
    Int8* data = (Int8*)image->data();
    m_writer->writeBytes(data, image->width() * image->height() * 2);
}

u32 WiiSave::writeFile(WiiFile* file)
{
    u32 ret = 0x80;

    m_writer->writeInt32(0x03ADF17E);
    m_writer->writeInt32(file->length());
    m_writer->writeByte(file->permissions());
    m_writer->writeByte(file->attributes());
    m_writer->writeByte(file->type());

    u8 name[0x45];
    sillyRandom(name, 0x45);
    memcpy(name, file->filename().c_str(), file->filename().size());
    name[file->filename().size()] = '\0';
    m_writer->writeBytes((Int8*)name, 0x45);
    u8 iv[16];
    sillyRandom(iv, 0x10);
    m_writer->writeBytes((Int8*)iv, 0x10);
    u8 crap[0x20];
    sillyRandom(crap, 0x20);
    m_writer->writeBytes((Int8*)crap, 0x20);

    if (file->type() == WiiFile::File)
    {
        int roundedSize = (file->length() + 63) & ~63;
        u8* data = new u8[roundedSize];
        memset(data, 0, roundedSize);

        aes_set_key(sd_key);
        aes_encrypt(iv, file->data(), data, roundedSize);

        m_writer->writeBytes((Int8*)data, roundedSize);
        ret += roundedSize;
    }

    return ret;
}

void WiiSave::writeCerts(int filesSize, int ngId, u8* ngPriv, u8* ngSig, int ngKeyId)
{
    u8 sig[0x40];
    u8 ngCert[0x180];
    u8 apCert[0x180];
    u8* hash;
    u8 apPriv[30];
    u8 apSig[60];
    char signer[64];
    char name[64];
    u8* data;
    u32 dataSize;

	sprintf(signer, "Root-CA00000001-MS00000002");
	sprintf(name, "NG%08x", ngId);
	make_ec_cert(ngCert, ngSig, signer, name, ngPriv, ngKeyId);

	memset(apPriv, 0, 30);
	apPriv[10] = 1;

	memset(apSig, 81, 30);

	sprintf(signer, "Root-CA00000001-MS00000002-NG%08x", ngId);
	sprintf(name, "AP%08x%08x", 1, 2);
	make_ec_cert(apCert, apSig, signer, name, apPriv, 0);

	hash = getSha1(apCert + 0x80, 0x100);
	generate_ecdsa(apSig, apSig+30, ngPriv, hash);
	make_ec_cert(apCert, apSig, signer, name, apPriv, 0);
	delete[] hash;

	dataSize = filesSize + 0x80;
	data = new u8[dataSize];
	u8* rawData = m_writer->data();
	memcpy(data, rawData + 0xF0C0, dataSize);

	hash = getSha1(data, dataSize);
	u8* hash2 = getSha1(hash, 20);
	delete[] hash;
	delete[] data;

	generate_ecdsa(sig, sig+30, apPriv, hash2);
	int stuff = 0x2f536969;
	if (!isSystemBigEndian())
        stuff = swap32(stuff);

	*(u32*)(sig+60) = stuff;
	delete[] hash2;

	m_writer->writeBytes((Int8*)sig, 0x40);
	m_writer->writeBytes((Int8*)ngCert, 0x180);
	m_writer->writeBytes((Int8*)apCert, 0x180);
}
