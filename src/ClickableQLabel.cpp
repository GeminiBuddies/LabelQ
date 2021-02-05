#include <ClickableQLabel.h>

#include <QEvent>
#include <QMouseEvent>

ClickableQLabel::ClickableQLabel(const QString &text, QWidget *parent) : QLabel(text, parent) {}

bool ClickableQLabel::event(QEvent *myEvent) {
    if (myEvent->type() == QEvent::MouseButtonRelease) {
        auto me = dynamic_cast<QMouseEvent *>(myEvent);

        if (me != nullptr && me->button() == Qt::LeftButton) {
            emit clicked();
            myEvent->accept();
            return true;
        }
    }

    return QLabel::event(myEvent);
}
