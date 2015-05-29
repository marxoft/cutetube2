#ifndef MASKEDITEM_H
#define MASKEDITEM_H

#include "maskeffect.h"
#include <QDeclarativeItem>

class MaskEffect;
class QDeclarativeComponent;


class MaskedItem : public QDeclarativeItem
{
    Q_OBJECT

    Q_PROPERTY(QDeclarativeComponent *mask
               READ mask
               WRITE setMask
               NOTIFY maskChanged)

public:
    MaskedItem(QDeclarativeItem *parent = 0);
    virtual ~MaskedItem();

    QDeclarativeComponent *mask() const;
    void setMask(QDeclarativeComponent *component);

signals:
    void maskChanged();

private:
    MaskEffect *m_effect;
    QDeclarativeComponent *m_maskComponent;
};

#endif // MASKEDITEM_H
