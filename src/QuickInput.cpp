#include <QuickInput.h>

#include <ClickableQLabel.h>

#include <QKeyEvent>
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


static const QVector<QString>& ords() {
    static QVector<QString> o = {
            "(1)", "(2)", "(3)", "(4)", "(5)", "(6)", "(7)", "(8)",
            "(9)", "(0)", "(q)", "(w)", "(e)", "(r)", "(t)", "(y)",
            "(u)", "(i)", "(o)", "(p)", "(a)", "(s)", "(d)", "(f)",
            "(g)", "(h)", "(j)", "(k)", "(l)", "(z)", "(x)", "(c)",
            "(v)", "(b)", "(n)", "(m)",
    };

    return o;
}

static const QVector<Qt::Key>& keys() {
    static QVector<Qt::Key> k = {
            Qt::Key_1, Qt::Key_2, Qt::Key_3, Qt::Key_4, Qt::Key_5, Qt::Key_6, Qt::Key_7, Qt::Key_8,
            Qt::Key_9, Qt::Key_0, Qt::Key_Q, Qt::Key_W, Qt::Key_E, Qt::Key_R, Qt::Key_T, Qt::Key_Y,
            Qt::Key_U, Qt::Key_I, Qt::Key_O, Qt::Key_P, Qt::Key_A, Qt::Key_S, Qt::Key_D, Qt::Key_F,
            Qt::Key_G, Qt::Key_H, Qt::Key_J, Qt::Key_K, Qt::Key_L, Qt::Key_Z, Qt::Key_X, Qt::Key_C,
            Qt::Key_V, Qt::Key_B, Qt::Key_N, Qt::Key_M,
    };

    return k;
};

QuickInput::QuickInput(QWidget *parent, const QStringList &words) : QDialog(parent) {
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);

    this->words = words;

    auto maxWordCount = ords().size();
    wordCount = words.size();

    if (wordCount > maxWordCount) {
        qWarning() << wordCount << " word(s) provided to QuickInput, but QuickInput can only handle " << maxWordCount << " word(s)";
        wordCount = maxWordCount;
    }

    auto l = new QuickInputLayout(this, w);

    for (int i = 0; i < wordCount; ++i) {
        auto label = new ClickableQLabel(QString("<span style=\"color: 'red'\">") + ords().at(i) + " </span>" + words.at(i));

        QObject::connect(label, &ClickableQLabel::clicked, [this, i](){
            selected = i;
            close();
        });

        l->addWidget(label);
    }

    auto size = l->sizeHint();
    qDebug() << "QuickInput popup size: " << size;

    resize(size);

    this->setLayout(l);
}

void QuickInput::keyPressEvent(QKeyEvent *myEvent) {
    if (myEvent->modifiers() == Qt::NoModifier) {
        auto key = myEvent->key();
        auto index = keys().indexOf((Qt::Key)key);

        if (index >= 0 && index < wordCount) {
            selected = index;
            close();
            return;
        }
    }

    QDialog::keyPressEvent(myEvent);
}

const QString &QuickInput::popup(const QPoint &pos) {
    selected = -1;

    this->move(pos);
    this->exec();

    return selected >= 0 && selected < wordCount ? words.at(selected) : emptyStr;
}
