#ifndef SKYWARDFACTORY_H
#define SKYWARDFACTORY_H


#include <QObject>
#include "savefactorybase.h"

class SaveBase;
namespace SkywardSword
{
class SkywardFactory : public QObject, public SaveFactoryBase
{
    Q_OBJECT
public:
    SkywardFactory();

    SaveBase* loadFile(const QString &filepath) {}
};
};
#endif // SKYWARDFACTORY_H
