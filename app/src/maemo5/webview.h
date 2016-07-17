#ifndef WEBVIEW_H
#define WEBVIEW_H

#include "qwebviewselectionsuppressor.h"
#include <QWebView>

class WebView : public QWebView
{
    Q_OBJECT

public:
    explicit WebView(QWidget *parent = 0);

public slots:
    void setNetworkAccessManager(QNetworkAccessManager *manager);
    void enableSelectionSuppressor() const;
    void disableSelectionSuppressor() const;
    void setSelectionSuppressorEnabled(bool enable) const;
    
private:
    QWebViewSelectionSuppressor *m_suppressor;
};

#endif // WEBVIEW_H
