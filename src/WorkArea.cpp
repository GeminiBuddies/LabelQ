#include <WorkArea.h>

#include <model/PageOperator.h>

#include <QEvent>
#include <QMessageBox>
#include <QMouseEvent>

#include <cmath>

WorkArea::WorkArea(QWidget *parent) : QLabel(parent), f("DejaVu Sans Mono") {
    currentZoomLevelIndex = 0;
    containerWidget = nullptr;
    op = nullptr;
    suppressExternalSignal = false;

    f.setBold(true);
}

void WorkArea::setContainerWidget(QWidget *widget) {
    containerWidget = widget;
}

double WorkArea::currentZoomLevel() const {
    return zoomLevel(currentZoomLevelIndex);
}

void WorkArea::setZoomLevel(int newIndex, bool forced) {
    if (newIndex < MinimumZoomLevelIndex || newIndex > MaximumZoomLevelIndex || newIndex == currentZoomLevelIndex) {
        if (!forced) return;

        newIndex = std::max(MinimumZoomLevelIndex, newIndex);
        newIndex = std::min(MaximumZoomLevelIndex, newIndex);
    }

    currentZoomLevelIndex = newIndex;

    if (isHidden()) {
        return;
    }

    // hide it first to avoid flickering
    this->hide();

    auto ratio = currentZoomLevel();
    auto newSize = pic.size() * ratio;
    setPixmap(pic.scaled(newSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

    // to fix an unknown bug that workArea->width/height() return wrong value when zooming in
    this->setFixedSize(newSize);

    auto x = std::min(newSize.width(), newSize.height());
    currentLabelWidgetSize = (int) (x * LabelWidgetSizeRatio);
    f.setPointSizeF(x * LabelWidgetFontSizeRation);

    auto labelCount = op->page()->labelCount();
    auto labelSize = (int) (x * LabelWidgetSizeRatio);
    for (int i = 0; i < labelCount; ++i) {
        auto w = labelWidgets[i];
        auto label = op->page()->label(i);

        w->setGeometry(QRect(label.position * ratio, QSize(labelSize, labelSize)));
        w->setFont(f);
    }

    this->show();

    emit sizeChanged();
}

void WorkArea::setPreferredZoomLevel() {
    // replace following content with:
    // setZoomLevel(0);
    // to disable

    if (this->isHidden()) {
        setZoomLevel(0);
        return;
    }

    auto parentSize = containerWidget->size();
    auto preferredZoomVertical = 5.0 / 6.0 * parentSize.height() / pic.height();
    auto preferredZoomHorizontal = 5.0 / 6.0 * parentSize.width() / pic.width();
    auto preferredZoom = std::max(preferredZoomVertical, preferredZoomHorizontal);
    preferredZoom = std::min(preferredZoom, zoomLevel(MaximumZoomLevelIndex));

    auto preferredIndex = std::lower_bound(ZoomLevels, ZoomLevels + ZoomLevelCount, preferredZoom) - ZoomLevels + MinimumZoomLevelIndex;

    setZoomLevel(preferredIndex, true);
}

void WorkArea::zoom(bool in) {
    setZoomLevel(currentZoomLevelIndex + (in ? 1 : -1));
}

void WorkArea::zoomReset() {
    setZoomLevel(0);
}

void WorkArea::markLabelWidgetAsSelected(QPushButton *widget) {
    widget->setStyleSheet("QPushButton { border: 2px solid gray; border-radius: 4px; background-color: red; color: white; }");
}

void WorkArea::markLabelWidgetAsUnselected(QPushButton *widget) {
    widget->setStyleSheet("QPushButton { border: 2px solid gray; border-radius: 4px; background-color: yellow; }");
}

QPushButton* WorkArea::getFreeLabelWidget() {
    QPushButton *rv;

    if (unusedLabelWidgets.count() > 0) {
        rv = unusedLabelWidgets.pop();
    } else {
        rv = new QPushButton(this);
        rv->installEventFilter(this);
    }

    markLabelWidgetAsUnselected(rv);
    rv->setFont(f);
    rv->setGeometry(0, 0, currentLabelWidgetSize, currentLabelWidgetSize);

    return rv;
}

QPushButton* WorkArea::addLabelWidget(const QPoint &position) {
    auto realPosition = position / currentZoomLevel();

    suppressExternalSignal = true;
    op->appendLabel(realPosition);
    suppressExternalSignal = false;

    auto rv = getFreeLabelWidget();
    labelWidgets.append(rv);

    rv->move(position);
    rv->setText(QString::number(labelWidgets.count()));
    rv->show();

    return rv;
}

void WorkArea::mousePressEvent(QMouseEvent *ev) {
    if (ev->button() == Qt::LeftButton && ev->modifiers() == Qt::NoModifier) {
        addLabelWidget(ev->pos());
    } else {
        QLabel::mousePressEvent(ev);
    }
}

// this function handles label widget click event
bool WorkArea::eventFilter(QObject *obj, QEvent *ev) {
    if (ev->type() == QEvent::MouseButtonPress) {
        auto index = labelWidgets.indexOf((QPushButton*)obj);

        if (index < 0) {
            return false;
        }

        QMessageBox::information(this, "", QString::number(index));
        return true;
    }

    return false;
}

void WorkArea::setPageOperator(PageOperator *op) {
    if (this->op != nullptr) {
        return;
    }

    this->op = op;

    QObject::connect(op, SIGNAL(newPageSet()), this, SLOT(onNewPage()));
    // QObject::connect(op, SIGNAL(labelAppended()), this, SLOT(onLabelAppended()));
    QObject::connect(op, SIGNAL(labelDeleted(QBitArray)), this, SLOT(onLabelDeleted(QBitArray)));
    QObject::connect(op, SIGNAL(labelSelectionUpdated(QBitArray)), this, SLOT(onLabelSelectionUpdated(QBitArray)));
}

void WorkArea::onNewPage() {
    auto newPage = op->page();

    if (newPage == nullptr) {
        this->hide();
    } else {
        this->show();
        pic = newPage->pixmap();
        this->setPixmap(pic);
    }

    auto oldLabelCount = labelWidgets.count();
    auto newLabelCount = newPage == nullptr ? 0 : newPage->labelCount();

    for (int i = oldLabelCount; i < newLabelCount; ++i) {
        auto rv = getFreeLabelWidget();
        labelWidgets.append(rv);
        rv->setText(QString::number(i + 1));
        rv->show();
    }

    for (int i = oldLabelCount - 1; i >= newLabelCount; --i) {
        auto w = labelWidgets[i];
        labelWidgets.remove(i);

        w->setVisible(false);
        unusedLabelWidgets.push(w);
    }

    setPreferredZoomLevel();
}

// void WorkArea::onLabelAppended() {}

void WorkArea::onLabelDeleted(QBitArray deleted) {
    if (suppressExternalSignal) {
        return;
    }

    auto oldCount = labelWidgets.count();
    for (int i = oldCount - 1; i >= 0; --i) {
        if (deleted.testBit(i)) {
            auto w = labelWidgets[i];
            labelWidgets.removeAt(i);

            w->setVisible(false);
            unusedLabelWidgets.push(w);
        }
    }

    auto newCount = labelWidgets.count();
    for (int i = 0; i < newCount; ++i) {
        labelWidgets[i]->setText(QString::number(i));
    }
}

void WorkArea::onLabelSelectionUpdated(QBitArray selected) {
    if (suppressExternalSignal) {
        return;
    }

    auto count = selected.count();

    for (int i = 0; i < count; ++i) {
        if (selected.testBit(i)) {
            markLabelWidgetAsSelected(labelWidgets[i]);
        } else {
            markLabelWidgetAsUnselected(labelWidgets[i]);
        }
    }
}
