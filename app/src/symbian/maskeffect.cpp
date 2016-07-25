#include "maskeffect.h"

MaskEffect::MaskEffect(QObject *parent)
    : QGraphicsEffect(parent)
{
}

void MaskEffect::setMask(QDeclarativeItem *mask)
{
    if (!m_item.isNull()) {
        disconnect(m_item.data(), SIGNAL(widthChanged()), this, SLOT(update()));
        disconnect(m_item.data(), SIGNAL(heightChanged()), this, SLOT(update()));
    }

    m_item.reset(mask);
    update();

    if (!m_item.isNull()) {
        connect(m_item.data(), SIGNAL(widthChanged()), this, SLOT(update()));
        connect(m_item.data(), SIGNAL(heightChanged()), this, SLOT(update()));
    }
}

void MaskEffect::draw(QPainter *painter)
{
    if (m_item.isNull()) {
        drawSource(painter);
        return;
    }

    QPoint offset;
    const QPixmap &pixmap = sourcePixmap(Qt::LogicalCoordinates,
                                         &offset, QGraphicsEffect::NoPad);

    if (pixmap.isNull())
        return;

    if (pixmap.size() != m_buffer.size())
        m_buffer = pixmap;

    QPainter p(&m_buffer);

    // clear buffer and set mask
    p.setCompositionMode(QPainter::CompositionMode_Source);
    p.fillRect(0, 0, m_buffer.width(), m_buffer.height(), Qt::transparent);
    m_item->paint(&p, 0, 0);

    // compose pixmap with mask in buffer
    p.setCompositionMode(QPainter::CompositionMode_SourceIn);
    p.drawPixmap(0, 0, pixmap);

    // draw composed buffer
    painter->drawPixmap(offset, m_buffer);
}
