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
    void        setData(const QByteArray& data);
    QByteArray data();
    void        setChecksum(quint32 checksum);
    quint32     checksum() const;
protected:
    // This holds the data for the save editor, allowing for easy use of QUndoStack for everything
    QHexEdit* m_hexData;
    quint32   m_checksum;
};

#endif // ADVENTUREBASE_H
