#ifndef WIISAVE_H
#define WIISAVE_H

#include <map>
#include <string>
#include <Types.hpp>

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
    bool     loadFromMemory(const Uint8* data, Uint64 length);

    bool     saveToFile(const std::string& filepath, Uint8* macAddress, Uint32 ngId, Uint8* ngPriv, Uint8* ngSig, Uint32 ngKeyId);

    void     addFile(const std::string& filename, WiiFile* file);
    WiiFile* getFile(const std::string& filename) const;
    std::map<std::string, WiiFile*>& getFileList();

    void     setBanner(WiiBanner* banner);
    WiiBanner* banner() const;

protected:
private:

    WiiBanner* readBanner();
    WiiFile*   readFile();
    WiiImage*  readImage(Uint32 width, Uint32 height);
    void       readCerts(Uint32 totalSize);

    void       writeBanner();
    Uint32     writeFile(WiiFile* file);
    void       writeImage(WiiImage* image);
    void       writeCerts(Uint32 filesSize, Uint32 ngId, Uint8* ngPriv, Uint8* ngSig, Uint32 ngKeyId);

    std::string readNullTermString();

    std::map<std::string, WiiFile*> m_files;
    WiiBanner* m_banner;

    BinaryReader* m_reader;
    BinaryWriter* m_writer;
};

#endif // WIISAVE_H
