#include "adventurebase.h"

#include "qhexedit2/qhexedit.h"
#include <QUndoStack>

QUndoStack* AdventureBase::undoStack() const
{
    return m_hexData->undoStack();
}
