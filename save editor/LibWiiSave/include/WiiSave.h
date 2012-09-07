#ifndef WIISAVE_H
#define WIISAVE_H

#include <map>
#include <string>
#include "tools.h"

class WiiFile;
class WiiBanner;
class WiiImage;
class BinaryReader;
class BinaryWriter;

class WiiSave
{
public:
    typedef std::map<std::string, WiiFile*>::const_iterator FileIterator;
    WiiSave();
    WiiSave(const std::string& filename);
    virtual ~WiiSave();

    bool     loadFromFile(const std::string& filename);
    bool     loadFromMemory(const u8* data, u64 length);

    bool     saveToFile(const std::string& filepath, u8* macAddress, int ngId, u8* ngPriv, u8* ngSig, int ngKeyId);

    void     addFile(const std::string& filename, WiiFile* file);
    WiiFile* getFile(const std::string& filename) const;
    std::map<std::string, WiiFile*>& getFileList();

    void     setBanner(WiiBanner* banner);
    WiiBanner* banner() const;

protected:
private:

    WiiBanner* readBanner();
    WiiFile*   readFile();
    WiiImage*  readImage(int width, int height);
    void       readCerts(u32 totalSize);

    void       writeBanner();
    u32        writeFile(WiiFile* file);
    void       writeImage(WiiImage* image);
    void       writeCerts(int filesSize, int ngId, u8* ngPriv, u8* ngSig, int ngKeyId);

    std::string readNullTermString();

    std::map<std::string, WiiFile*> m_files;
    WiiBanner* m_banner;

    BinaryReader* m_reader;
    BinaryWriter* m_writer;
};

#endif // WIISAVE_H
