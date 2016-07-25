import QtQuick 1.1
import com.nokia.meego 1.0

BusyIndicator {
    property alias size: style.size
    property alias inverted: style.inverted

    running: visible
    platformStyle: BusyIndicatorStyle {
        id: style
    }
}
