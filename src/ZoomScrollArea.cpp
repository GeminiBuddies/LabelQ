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

void ZoomScrollArea::keyPressEvent(QKeyEvent *event) {
    while (event->modifiers() == Qt::ControlModifier) {
        auto key = event->key();

        if (key == Qt::Key_Equal) {
            emit zoom(true);
        } else if (key == Qt::Key_Minus) {
            emit zoom(false);
        } else if (key == Qt::Key_0) {
            emit zoomReset();
        } else {
            break;
        }

        event->accept();
        return;
    }

    QScrollArea::keyPressEvent(event);
}
