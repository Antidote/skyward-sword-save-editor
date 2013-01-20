#include "WiiBanner.h"
#include <utility.hpp>
#include <string.h>

WiiImage::WiiImage(Uint32 width, Uint32 height, Uint8* data) :
    m_width(width),
    m_height(height),
    m_data(data)
{
}
WiiImage::~WiiImage()
{
    if (m_data)
        delete[] m_data;
    m_data = NULL;
}

Uint8* WiiImage::data()
{
    return m_data;
}

Uint32 WiiImage::width() const
{
    return m_width;
}

Uint32 WiiImage::height() const
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

WiiBanner::WiiBanner(Uint32 gameId, const std::string& title,
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

void WiiBanner::setGameID(Uint64 id)
{
    m_gameId = id;
}

Uint64 WiiBanner::gameID() const
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

void WiiBanner::setIcon(Uint32 id, WiiImage* icon)
{
    if (m_icons[id] != NULL)
    {
        delete m_icons[id];
        m_icons[id] = icon;
    }
}

WiiImage* WiiBanner::getIcon(Uint32 id) const
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

void WiiBanner::setAnimationSpeed(Uint16 animSpeed)
{
    m_animSpeed = animSpeed;
}

Uint16 WiiBanner::animationSpeed() const
{
    return m_animSpeed;
}

void WiiBanner::setPermissions(Uint8 permissions)
{
    m_permissions = permissions;
}

Uint8 WiiBanner::permissions() const
{
    return m_permissions;
}

void WiiBanner::setBannerSize(Uint32 size)
{
    m_bannerSize = size;
}

Uint32 WiiBanner::bannerSize() const
{
    return m_bannerSize;
}

void WiiBanner::setFlags(Uint32 flags)
{
    m_flags = flags;
}

Uint32 WiiBanner::flags() const
{
    return m_flags;
}

