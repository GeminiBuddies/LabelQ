#include <ZoomScrollArea.h>

ZoomScrollArea::ZoomScrollArea(QWidget *parent) : QScrollArea(parent) {}

ZoomScrollArea::~ZoomScrollArea() = default;

void ZoomScrollArea::wheelEvent(QWheelEvent *event) {
    if (event->modifiers() == Qt::ControlModifier) {
        emit zoom(event->angleDelta().y() > 0);
        event->accept();
    } else {
        QScrollArea::wheelEvent(event);
    }
}
