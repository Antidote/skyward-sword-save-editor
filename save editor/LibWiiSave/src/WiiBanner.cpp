#include "WiiBanner.h"
#include "utility.h"
#include <string.h>
#include <QDebug>


WiiImage::WiiImage(u32 width, u32 height, u8* data) :
    m_width(width),
    m_height(height),
    m_data(data)
{
}

u8* WiiImage::data()
{
    return m_data;
}

u32 WiiImage::width() const
{
    return m_width;
}

u32 WiiImage::height() const
{
    return m_height;
}

WiiBanner::WiiBanner() :
    m_gameId(0),
    m_banner(NULL),
    m_flags(0),
    m_bannerSize(0),
    m_title(""),
    m_subtitle("")
{
}

WiiBanner::WiiBanner(int gameId, const std::string& title,
                     const std::string& subtitle, WiiImage* banner, std::vector<WiiImage*> icons) :
    m_gameId(gameId),
    m_banner(banner),
    m_flags(0),
    m_bannerSize(0),
    m_icons(icons),
    m_title(title),
    m_subtitle(subtitle)
{
}

WiiBanner::~WiiBanner()
{
    delete m_banner;
    m_icons.clear();
}

void WiiBanner::setGameID(u64 id)
{
    m_gameId = id;
}

u64 WiiBanner::gameID() const
{
    return m_gameId;
}
void WiiBanner::setTitle(const std::string& title)
{
    m_title = title;
}

std::string WiiBanner::title() const
{
    return m_title;
}

void WiiBanner::setSubtitle(const std::string& subtitle)
{
    m_subtitle = subtitle;
}

std::string WiiBanner::subtitle() const
{
    return m_subtitle;
}

void WiiBanner::addIcon(WiiImage* icon)
{
    m_icons.push_back(icon);
}

void WiiBanner::setIcon(int id, WiiImage* icon)
{
    if (m_icons[id] != NULL)
    {
        delete m_icons[id];
        m_icons[id] = icon;
    }
}

WiiImage* WiiBanner::getIcon(int id) const
{
    if (!m_icons[id])
        return NULL;

    return m_icons[id];
}
std::vector<WiiImage*> WiiBanner::icons() const
{
    return m_icons;
}

void WiiBanner::setBannerImage(WiiImage* banner)
{
    m_banner = banner;
}

WiiImage* WiiBanner::bannerImage() const
{
    return m_banner;
}

void WiiBanner::setAnimationSpeed(short animSpeed)
{
    m_animSpeed = animSpeed;
}

short WiiBanner::animationSpeed() const
{
    return m_animSpeed;
}

void WiiBanner::setPermissions(u8 permissions)
{
    m_permissions = permissions;
}

u8 WiiBanner::permissions() const
{
    return m_permissions;
}

void WiiBanner::setBannerSize(u32 size)
{
    m_bannerSize = size;
}

u32 WiiBanner::bannerSize() const
{
    return m_bannerSize;
}

void WiiBanner::setFlags(u32 flags)
{
    m_flags = flags;
}

u32 WiiBanner::flags() const
{
    return m_flags;
}

