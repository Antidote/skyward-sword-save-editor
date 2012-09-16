#ifndef WIIFILE_H
#define WIIFILE_H

#include <string>
#include <map>
#include "tools.h"

class WiiFile
{
public:
    enum Permission
    {
        OtherRead  = 0x01,
        OtherWrite = 0x02,
        GroupRead  = 0x04,
        GroupWrite = 0x08,
        OwnerRead  = 0x10,
        OwnerWrite = 0x20,

        // Mask values;
        OtherRW = (OtherRead|OtherWrite),
        GroupRW = (GroupRead|GroupWrite),
        OwnerRW = (OwnerRead|OwnerWrite)
    };

    enum Type
    {
        File      = 0x01,
        Directory = 0x02
    };

    WiiFile();
    WiiFile(const std::string& filename);
    WiiFile(const std::string& filename, u8 permissions, const u8* data, int length);
    virtual ~WiiFile();

    void setFilename(const std::string& filename);
    std::string filename() const;

    void setData(const u8* data);
    u8* data() const;

    void setLength(const int len);
    int  length() const;

    void setPermissions(const u8 permissions);
    u8 permissions() const;

    void setAttributes(const u8 attr);
    u8 attributes() const;

    void setType(Type type);
    Type type() const;

    bool isDirectory() const;
    bool isFile() const;

protected:
private:
    u8        m_permissions;
    u8        m_attributes;
    Type        m_type;
    std::string m_filename;
    int         m_fileLen;
    u8*       m_fileData;
};

#endif // WIIFILE_H
