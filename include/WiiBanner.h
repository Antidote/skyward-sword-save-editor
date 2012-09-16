#ifndef WIIBANNER_H
#define WIIBANNER_H


#include <vector>
#include <string>
#include "tools.h"

class WiiImage
{
public:
    WiiImage();
    WiiImage(u32 width, u32 height, u8* data);
    //~WiiImage();

    void  setWidth(const int width);
    u32   width() const;

    void  setHeight(const int height);
    u32   height() const;

    void  setData(const u8* data);
    u8* data();

    u8* toRGBA32();

private:
    int   m_width;
    int   m_height;
    u8* m_data;
};

class WiiBanner
{
public:
    enum { NoCopy = 0x00000001, Bounce = 0x00000010, NoCopyBounce = NoCopy | Bounce };
    WiiBanner();
    WiiBanner(int gameId, const std::string& title, const std::string& subtitle, WiiImage* m_banner, std::vector<WiiImage*> icons);
    virtual ~WiiBanner();

    void setGameID(u64 id);
    u64  gameID() const;

    void setBannerImage(WiiImage* banner);
    WiiImage* bannerImage() const;

    void setBannerSize(u32 size);
    u32 bannerSize() const;

    void setTitle(const std::string& title);
    std::string title() const;

    void setSubtitle(const std::string& subtitle);
    std::string subtitle() const;

    void addIcon(WiiImage* icon);
    void setIcon(int id, WiiImage* icon);
    WiiImage* getIcon(int id) const;
    std::vector<WiiImage*> icons() const;

    void setAnimationSpeed(short animSpeed);
    short animationSpeed() const;

    void setPermissions(u8 permissions);
    u8   permissions() const;

    void setFlags(u32 flags);
    u32  flags() const;
protected:
private:
    u64                    m_gameId;
    WiiImage*              m_banner;
    u32                    m_animSpeed;
    u8                     m_permissions;
    u32                    m_flags;
    u32                    m_bannerSize;
    std::vector<WiiImage*> m_icons;
    std::string            m_title;
    std::string            m_subtitle;
};

#endif // WIIBANNER_H
