#include "adventurebase.h"

#include "qhexedit2/qhexedit.h"
#include <QUndoStack>

QUndoStack* AdventureBase::undoStack() const
{
    return m_hexData->undoStack();
}

void AdventureBase::setData(const QByteArray &data)
{
    m_hexData->setData(data);
}

QByteArray AdventureBase::data()
{
    return m_hexData->data();
}

void AdventureBase::setChecksum(quint32 checksum)
{
    m_checksum = checksum;
}

quint32 AdventureBase::checksum() const
{
    return m_checksum;
}
