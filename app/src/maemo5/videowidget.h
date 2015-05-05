#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QVideoWidget>

class VideoWidget : public QVideoWidget
{
    Q_OBJECT

public:
    explicit VideoWidget(QWidget *parent = 0);

public Q_SLOTS:
    void toggleAspectRatioMode();

protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

Q_SIGNALS:
    void clicked();

private:
    bool m_pressed;
};

#endif // VIDEOWIDGET_H
