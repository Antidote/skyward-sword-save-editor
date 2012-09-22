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
    virtual void close() = 0;
    virtual bool reload(int adventureIndex) = 0;
    virtual bool save(const QString& filename = NULL) = 0;
    virtual bool open(const QString& filepath=NULL) = 0;
    virtual void createNewGame(int index) = 0;
    virtual void deleteAdventure(AdventureBase*) = 0;
    virtual void deleteAllGames() = 0;
    virtual bool isModified() const = 0;

    QString filename() const;
    void setFilename(const QString& filepath);

private:
    QString m_filename;

signals:
    void loadFailed(const QString&, const QString&);
    void saveFailed(const QString&, const QString&);
};

#endif // IGAMEFILE_H
