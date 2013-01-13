#include "WiiFile.h"

WiiFile::WiiFile() :
    m_permissions(WiiFile::GroupRW|WiiFile::OtherRW|WiiFile::OwnerRW),
    m_attributes(0),
    m_type(WiiFile::File),
    m_filename(""),
    m_fileLen(0),
    m_fileData(NULL)
{
    //ctor
}

WiiFile::WiiFile(const std::string& filename) :
    m_permissions(WiiFile::GroupRW|WiiFile::OtherRW|WiiFile::OwnerRW),
    m_attributes(0),
    m_type(WiiFile::File),
    m_filename(filename),
    m_fileLen(0),
    m_fileData(NULL)
{
}

WiiFile::WiiFile(const std::string& filename, Uint8 permissions, const Uint8* data, Uint32 length) :
    m_permissions(permissions),
    m_attributes(0),
    m_type(WiiFile::File),
    m_filename(filename),
    m_fileLen(length),
    m_fileData((Uint8*)data)
{
}

WiiFile::~WiiFile()
{
    if (m_fileData)
        delete[] m_fileData;
}


void WiiFile::setFilename(const std::string& filename)
{
    m_filename = filename;
}

std::string WiiFile::filename() const
{
    return m_filename;
}

void WiiFile::setPermissions(const Uint8 permissions)
{
    m_permissions = (Uint8)permissions;
}

Uint8 WiiFile::permissions() const
{
    return m_permissions;
}

void WiiFile::setData(const Uint8* data)
{
    if (m_fileData)
    {
        delete[] m_fileData;
        m_fileData = NULL;
    }
    m_fileData = (Uint8*)data;
}

Uint8* WiiFile::data() const
{
    return m_fileData;
}

void WiiFile::setLength(const int len)
{
    m_fileLen = (int)len;
}

int WiiFile::length() const
{
    return m_fileLen;
}

void WiiFile::setAttributes(const Uint8 attr)
{
    m_attributes = attr;
}

Uint8 WiiFile::attributes() const
{
    return m_attributes;
}

void WiiFile::setType(WiiFile::Type type)
{
    m_type = type;
}

WiiFile::Type WiiFile::type() const
{
    return m_type;
}

bool WiiFile::isDirectory() const
{
    return (m_type == WiiFile::Directory);
}

bool WiiFile::isFile() const
{
    return (m_type == WiiFile::File);
}

