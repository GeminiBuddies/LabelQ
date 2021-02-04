#include <QuickInput.h>

QuickInput::QuickInput(QWidget *parent) : QDialog(parent) {
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    resize(150, 90);
}
