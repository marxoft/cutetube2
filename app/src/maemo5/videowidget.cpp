#include "videowidget.h"
#include <QMouseEvent>

VideoWidget::VideoWidget(QWidget *parent) :
    QVideoWidget(parent)
{
    setFocusPolicy(Qt::NoFocus);
}

void VideoWidget::mousePressEvent(QMouseEvent *e) {
    m_pressed = true;
    e->accept();
}

void VideoWidget::mouseReleaseEvent(QMouseEvent *e) {
    if (m_pressed) {
        emit clicked();
        e->accept();
    }

    m_pressed = false;
}

void VideoWidget::toggleAspectRatioMode() {
    switch (aspectRatioMode()) {
    case Qt::KeepAspectRatio:
        setAspectRatioMode(Qt::KeepAspectRatioByExpanding);
        break;
    case Qt::KeepAspectRatioByExpanding:
        setAspectRatioMode(Qt::IgnoreAspectRatio);
        break;
    default:
        setAspectRatioMode(Qt::KeepAspectRatio);
        break;
    }
}
