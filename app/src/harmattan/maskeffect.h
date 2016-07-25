#ifndef MASKEFFECT_H
#define MASKEFFECT_H

#include <QEvent>
#include <QPainter>
#include <QScopedPointer>
#include <QGraphicsEffect>
#include <QDeclarativeComponent>
#include <QDeclarativeItem>


class MaskEffect : public QGraphicsEffect
{
    Q_OBJECT

public:
    MaskEffect(QObject *parent = 0);

    void setMask(QDeclarativeItem *mask);

protected:
    void draw(QPainter *painter);

private:
    QPixmap m_buffer;
    QScopedPointer<QDeclarativeItem> m_item;
};

#endif // MASKEFFECT_H
