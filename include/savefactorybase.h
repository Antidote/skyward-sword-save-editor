#ifndef SAVEFACTORYBASE_H
#define SAVEFACTORYBASE_H

#include <QString>

class SaveBase;

class SaveFactoryBase
{
public:
    SaveFactoryBase();

    virtual QString validFormats()=0;

    virtual SaveBase* loadFile(const QString& filepath);
};

#endif // SAVEFACTORYBASE_H
