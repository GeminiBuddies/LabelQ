#include <QuickInput.h>

#include <QLabel>
#include <QDebug>

QuickInputLayout::QuickInputLayout(QWidget *parent, int w) : QLayout(parent), w(w) {

}

void QuickInputLayout::addItem(QLayoutItem *item) {
    items.append(item);
}

int QuickInputLayout::count() const {
    return items.count();
}

QLayoutItem *QuickInputLayout::itemAt(int index) const {
    return items.at(index);
}

QLayoutItem *QuickInputLayout::takeAt(int index) {
    if (index < 0 || index >= items.count()) {
        return nullptr;
    }

    return items.takeAt(index);
}

QSize QuickInputLayout::minimumSize() const {
    return doLayout(QRect(0, 0, w, 0), true);
}

QSize QuickInputLayout::sizeHint() const {
    return minimumSize();
}

void QuickInputLayout::setGeometry(const QRect &rect) {
    QLayout::setGeometry(rect);
    doLayout(rect, false);
}

QSize QuickInputLayout::doLayout(const QRect &rect, bool calcOnly) const {
    // we use code from qt example 'flow layout' temporarily
    int left, top, right, bottom;
    // getContentsMargins(&left, &top, &right, &bottom);
    left = top = right = bottom = 8;
    QRect effectiveRect = rect.adjusted(+left, +top, -right, -bottom);
    int x = effectiveRect.x();
    int y = effectiveRect.y();
    int lineHeight = 0;

    for (QLayoutItem *item : items) {
        const QWidget *wid = item->widget();
        int spaceX = 8;
        int spaceY = 8;

        int nextX = x + item->sizeHint().width() + spaceX;
        if (nextX - spaceX > effectiveRect.right() && lineHeight > 0) {
            x = effectiveRect.x();
            y = y + lineHeight + spaceY;
            nextX = x + item->sizeHint().width() + spaceX;
            lineHeight = 0;
        }

        if (!calcOnly)
            item->setGeometry(QRect(QPoint(x, y), item->sizeHint()));

        x = nextX;
        lineHeight = qMax(lineHeight, item->sizeHint().height());
    }

    return QSize(w, y + lineHeight - rect.y() + bottom);
}


static QVector<QString>& ords() {
    static QVector<QString> v = {
            "⑴", "⑵", "⑶", "⑷", "⑸", "⑹", "⑺", "⑻",
            "⑼", "⒬", "⒲", "⒠", "⒭", "⒯", "⒴", "⒰",
            "⒤", "⒪", "⒫", "⒜", "⒮", "⒟", "⒡", "⒢",
            "⒣", "⒥", "⒦", "⒧", "⒵", "⒳", "⒞", "⒱",
            "⒝", "⒩", "⒨",
    };

    return v;
}

QuickInput::QuickInput(QWidget *parent) : QDialog(parent) {
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);

    auto maxWords = ords().size();

    auto l = new QuickInputLayout(this, w);

    l->addWidget(new QLabel("<font color='red'>⑴ </font>1111"));
    l->addWidget(new QLabel("⑵22222222"));
    l->addWidget(new QLabel("333333333333"));
    l->addWidget(new QLabel("4444444444444444"));
    l->addWidget(new QLabel("55555555555555555555"));
    l->addWidget(new QLabel("666666666666666666666666"));
    l->addWidget(new QLabel("7777777777777777777777777777"));
    l->addWidget(new QLabel("88888888888888888888888888888888"));
    l->addWidget(new QLabel("999999999999999999999999999999999999"));

    auto size = l->sizeHint();
    qDebug() << size;

    resize(size);

    this->setLayout(l);
}
