#include <ZoomScrollArea.h>

ZoomScrollArea::ZoomScrollArea(QWidget *parent) : QScrollArea(parent) {}

ZoomScrollArea::~ZoomScrollArea() = default;

void ZoomScrollArea::wheelEvent(QWheelEvent *ev) {
    if (ev->modifiers() == Qt::ControlModifier) {
        emit zoom(ev->angleDelta().y() > 0);
        ev->accept();
    } else if (ev->modifiers() == Qt::ShiftModifier) {
        if (ev->angleDelta().y() > 0) {
            emit prev();
        } else {
            emit next();
        }

        ev->accept();
    } else {
        QScrollArea::wheelEvent(ev);
    }
}

void ZoomScrollArea::mousePressEvent(QMouseEvent *ev) {
    if (ev->button() == Qt::MidButton && ev->modifiers() == Qt::ControlModifier) {
        emit zoomReset();
        ev->accept();
    } else {
        QScrollArea::mousePressEvent(ev);
    }
}

void ZoomScrollArea::keyPressEvent(QKeyEvent *ev) {
    auto key = ev->key();

    while (ev->modifiers() == Qt::ControlModifier) {
        if (key == Qt::Key_Equal) {
            emit zoom(true);
        } else if (key == Qt::Key_Minus) {
            emit zoom(false);
        } else if (key == Qt::Key_0) {
            emit zoomReset();
        } else {
            break;
        }

        ev->accept();
        return;
    }

    while (ev->modifiers() == Qt::ShiftModifier) {
        if (key == Qt::Key_Left) {
            emit next();
        } else if (key == Qt::Key_Right) {
            emit prev();
        } else {
            break;
        }

        ev->accept();
        return;
    }

    QScrollArea::keyPressEvent(ev);
}
