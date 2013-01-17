#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H


// Need to add more settings

#include <QObject>
#include <QStringList>

class SettingsManager : public QObject
{
    Q_OBJECT
public:
    virtual ~SettingsManager();

    enum {NTSCU, NTSCJ, PAL};
    
    QString defaultPlayerNameForRegion(quint32 region) const;
    void setDefaultPlayerNameForRegion(quint32 region, const QString& name);
    void setDefaultPlayerNameForRegion(const QStringList&);

    QString defaultPlayerName() const;

    quint32 defaultRegion() const;
    void setDefaultRegion(const quint32 region);

    void saveSettings();
    static SettingsManager* instance();
signals:
    
public slots:

private:
    SettingsManager* operator=(SettingsManager&);
    SettingsManager(SettingsManager&);
    SettingsManager();
    QStringList m_defaultNameList; //!< The default name for the region, Must be in order of NTSCU, NTSCJ, PAL
    quint32     m_defaultRegion;   //!< The default region for the save

    static SettingsManager* m_instance;
    static const ushort JAPANESE_NAME[4];
};

#endif // SETTINGSMANAGER_H
