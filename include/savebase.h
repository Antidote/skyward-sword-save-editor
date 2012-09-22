#ifndef IGAMEFILE_H
#define IGAMEFILE_H

#include <QObject>
#include <QString>

class AdventureBase;

class SaveBase : public QObject
{
    Q_OBJECT

public:
    virtual ~SaveBase(){}

    virtual QString gameName() const=0;

    virtual bool isOpen() const=0;
    virtual bool save() = 0 ;
    virtual bool open() = 0;
    bool isModified();

    QString filename() const;
    void    setFilename(const QString& filepath);

protected:
    QString m_filename;
    AdventureBase*        m_currentAdventure;
    QList<AdventureBase*> m_adventures;
signals:
    void loadFailed(const QString&, const QString&);
    void saveFailed(const QString&, const QString&);
};

#endif // IGAMEFILE_H
