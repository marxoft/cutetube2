#include "webview.h"
#include <QAbstractKineticScroller>

WebView::WebView(QWidget *parent) :
    QWebView(parent),
    m_suppressor(new QWebViewSelectionSuppressor(this))
{
}

void WebView::setNetworkAccessManager(QNetworkAccessManager *manager) {
    page()->setNetworkAccessManager(manager);
}

void WebView::enableSelectionSuppressor() const {
    m_suppressor->enable();
    property("kineticScroller").value<QAbstractKineticScroller *>()->setEnabled(true);
}

void WebView::disableSelectionSuppressor() const {
    m_suppressor->disable();
    property("kineticScroller").value<QAbstractKineticScroller *>()->setEnabled(false);
}

void WebView::setSelectionSuppressorEnabled(bool enable) const {
    if (enable) {
        enableSelectionSuppressor();
    }
    else {
        disableSelectionSuppressor();
    }
}
