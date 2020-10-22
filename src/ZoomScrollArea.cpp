#include <ZoomScrollArea.h>

ZoomScrollArea::ZoomScrollArea(QWidget *parent) : QScrollArea(parent) {}

ZoomScrollArea::~ZoomScrollArea() = default;

void ZoomScrollArea::wheelEvent(QWheelEvent *event) {
    if (event->modifiers() == Qt::ControlModifier) {
        if (event->angleDelta().y() < 0) {
            emit zoomOut();
        } else {
            emit zoomIn();
        }

        event->accept();
    } else {
        QScrollArea::wheelEvent(event);
    }
}
