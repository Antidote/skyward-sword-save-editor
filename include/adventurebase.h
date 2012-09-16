#ifndef ADVENTUREBASE_H
#define ADVENTUREBASE_H

#include <QObject>
class QHexEdit;
class QUndoStack;

class AdventureBase : public QObject
{
    Q_OBJECT
public:

    QUndoStack* undoStack() const;
protected:
    // This holds the data for the save editor, allowing for easy use of QUndoStack for everything
    QHexEdit* m_hexData;
};

#endif // ADVENTUREBASE_H
