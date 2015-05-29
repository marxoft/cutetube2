#include "maskeditem.h"

#include <QEvent>
#include <QPainter>
#include <QScopedPointer>
#include <QGraphicsEffect>
#include <QDeclarativeComponent>

MaskedItem::MaskedItem(QDeclarativeItem *parent)
    : QDeclarativeItem(parent),
      m_maskComponent(0)
{
    // XXX: this is a workaround to avoid a bug in
    // QGraphicsEffect that keeps dirty regions on
    // an item with no contents
    setFlag(QGraphicsItem::ItemHasNoContents, false);

    setFlag(QGraphicsItem::ItemClipsChildrenToShape);

    m_effect = new MaskEffect();
    setGraphicsEffect(m_effect);
}

MaskedItem::~MaskedItem()
{

}

QDeclarativeComponent *MaskedItem::mask() const
{
    return m_maskComponent;
}

void MaskedItem::setMask(QDeclarativeComponent *component)
{
    if (m_maskComponent == component)
        return;

    QDeclarativeItem *mask = 0;

    if (component) {
        QObject *object = component->create(component->creationContext());
        mask = qobject_cast<QDeclarativeItem *>(object);

        if (!mask)
            qWarning("MaskedItem: Unable to create mask element.");
        else if (!mask->childItems().isEmpty())
            qWarning("MaskedItem: Mask element has children. Due to current limitation, they won't be painted.");
    }

    m_effect->setMask(mask);
    m_maskComponent = component;

    emit maskChanged();
}
