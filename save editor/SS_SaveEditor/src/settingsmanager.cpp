#include "settingsmanager.h"
#include <QSettings>
#include <QStringList>

SettingsManager* SettingsManager::m_instance = NULL;
const ushort     SettingsManager::JAPANESE_NAME[4] = {0x30ea, 0x30f3, 0x30af, 0};

SettingsManager::SettingsManager()
    : QObject(NULL)
{
    m_defaultNameList = QStringList() << "Link" << QString::fromUtf16(JAPANESE_NAME) << "Link";
    m_defaultRegion   = NTSCU;

    QSettings settings;
    settings.beginGroup("General Settings");
    m_defaultNameList = settings.value("defaultPlayerNameForRegion", m_defaultNameList).toStringList();
    m_defaultRegion = (settings.value("defaultRegion", "NTSCU") == "NTSCU" ? NTSCU
                      : settings.value("defaultRegion", "NTSCJ") == "NTSCJ"? NTSCJ : PAL);
    settings.endGroup();
}

SettingsManager::~SettingsManager()
{
    saveSettings();
}


QString SettingsManager::defaultPlayerNameForRegion(quint32 region) const
{
    return m_defaultNameList[region];
}

void SettingsManager::setDefaultPlayerNameForRegion(quint32 region, const QString &name)
{
    m_defaultNameList[region] = name;
}

QString SettingsManager::defaultPlayerName() const
{
    return m_defaultNameList[m_defaultRegion];
}

quint32 SettingsManager::defaultRegion() const
{
    return m_defaultRegion;
}

void SettingsManager::setDefaultRegion(const quint32 region)
{
    if (region > PAL)
        return;

    m_defaultRegion = region;
}

void SettingsManager::saveSettings()
{
    QSettings settings;
    settings.beginGroup("General Settings");
    settings.setValue("defaultPlayerNameForRegion", m_defaultNameList);
    settings.setValue("defaultRegion", (m_defaultRegion == NTSCU ? "NTSCU" : (m_defaultRegion == NTSCJ ? "NTSCJ" : "PAL")));
    settings.endGroup();
}


SettingsManager *SettingsManager::instance()
{
    if (!m_instance)
        m_instance = new SettingsManager;

    return m_instance;
}
