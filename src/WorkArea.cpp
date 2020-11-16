#include <WorkArea.h>

#include <controller/PageOperator.h>

#include <QEvent>
#include <QMessageBox>
#include <QMouseEvent>

#include <cmath>

WorkArea::WorkArea(QWidget *parent) : QLabel(parent), labelFont("DejaVu Sans Mono") {
    currentZoomLevelIndex = 0;
    containerWidget = nullptr;
    op = nullptr;
    suppressExternalSignal = false;

    labelFont.setBold(true);
}

void WorkArea::setContainerWidget(QWidget *widget) {
    containerWidget = widget;
}

double WorkArea::currentZoomLevel() const {
    return zoomLevel(currentZoomLevelIndex);
}

void WorkArea::setZoomLevel(int newIndex, bool forced) {
    if (newIndex < ZoomLevelMinimumIndex || newIndex > ZoomLevelMaximumIndex || newIndex == currentZoomLevelIndex) {
        if (!forced) return;

        newIndex = std::max(ZoomLevelMinimumIndex, newIndex);
        newIndex = std::min(ZoomLevelMaximumIndex, newIndex);
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
    labelFont.setPointSizeF(x * LabelWidgetFontSizeRation);

    auto labelCount = op->page()->labelCount();
    auto labelSize = (int) (x * LabelWidgetSizeRatio);
    for (int i = 0; i < labelCount; ++i) {
        auto w = labelWidgets[i];
        auto label = op->page()->label(i);

        w->setGeometry(QRect(label.position * ratio, QSize(labelSize, labelSize)));
        w->setFont(labelFont);
    }

    this->show();

    emit sizeChanged();
}

void WorkArea::setPreferredZoomLevel() {
    // replace following content with:
    // setZoomLevel(0);
    // to disable auto-zoom

    if (this->isHidden()) {
        setZoomLevel(0);
        return;
    }

    auto containerSize = containerWidget->size();
    auto preferredZoomVertical = WorkAreaPreferredSizeRatio * containerSize.height() / pic.height();
    auto preferredZoomHorizontal = WorkAreaPreferredSizeRatio * containerSize.width() / pic.width();
    auto preferredZoom = std::min(std::max(preferredZoomVertical, preferredZoomHorizontal), zoomLevel(ZoomLevelMaximumIndex));

    auto preferredIndex = std::lower_bound(ZoomLevels, ZoomLevels + ZoomLevelCount, preferredZoom) - ZoomLevels + ZoomLevelMinimumIndex;

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
    rv->setFont(labelFont);
    rv->setGeometry(0, 0, currentLabelWidgetSize, currentLabelWidgetSize);
    rv->setToolTip("");

    return rv;
}

QPushButton* WorkArea::addLabelWidget(const QPoint &position) {
    auto realPosition = position / currentZoomLevel();

    labelSelection.resize(labelSelection.count() + 1);

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

void WorkArea::deleteLabel(const QBitArray &deleted, bool notify) {
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
        labelWidgets[i]->setText(QString::number(i + 1));
    }

    clearSelection(false);
    labelSelection.resize(newCount);

    if (notify) {
        suppressExternalSignal = true;
        op->deleteLabel(deleted);
        suppressExternalSignal = false;
    }
}

void WorkArea::clearSelection(bool notify) {
    auto len = labelWidgets.count();

    labelSelection.clear();
    labelSelection.resize(len);

    for (int i = 0; i < len; ++i) {
        markLabelWidgetAsUnselected(labelWidgets[i]);
    }

    if (notify) {
        suppressExternalSignal = true;
        op->setLabelSelection(labelSelection);
        suppressExternalSignal = false;
    }
}

void WorkArea::select(int index) {
    if (!labelSelection.testBit(index)) {
        labelSelection.setBit(index);
        markLabelWidgetAsSelected(labelWidgets[index]);

        suppressExternalSignal = true;
        op->setLabelSelection(labelSelection);
        suppressExternalSignal = false;
    }
}

void WorkArea::unselect(int index) {
    if (labelSelection.testBit(index)) {
        labelSelection.clearBit(index);
        markLabelWidgetAsUnselected(labelWidgets[index]);

        suppressExternalSignal = true;
        op->setLabelSelection(labelSelection);
        suppressExternalSignal = false;
    }
}

void WorkArea::toggle(int index) {
    if (labelSelection.testBit(index)) {
        labelSelection.clearBit(index);
        markLabelWidgetAsUnselected(labelWidgets[index]);
    } else {
        labelSelection.setBit(index);
        markLabelWidgetAsSelected(labelWidgets[index]);
    }

    suppressExternalSignal = true;
    op->setLabelSelection(labelSelection);
    suppressExternalSignal = false;
}

void WorkArea::setSelection(const QBitArray &selected, bool notify) {
    auto count = selected.count();

    for (int i = 0; i < count; ++i) {
        if (selected.testBit(i)) {
            markLabelWidgetAsSelected(labelWidgets[i]);
        } else {
            markLabelWidgetAsUnselected(labelWidgets[i]);
        }
    }

    labelSelection = selected;

    if (notify) {
        suppressExternalSignal = true;
        op->setLabelSelection(selected);
        suppressExternalSignal = false;
    }
}

void WorkArea::updateToolTip(int index) {
    labelWidgets[index]->setToolTip(op->page()->label(index).translation);
}

void WorkArea::mousePressEvent(QMouseEvent *ev) {
    if (ev->button() == Qt::LeftButton && ev->modifiers() == Qt::NoModifier) {
        clearSelection();
        addLabelWidget(ev->pos() - QPoint(currentLabelWidgetSize / 2, currentLabelWidgetSize / 2));
    } else {
        QLabel::mousePressEvent(ev);
    }
}

void WorkArea::keyPressEvent(QKeyEvent *ev) {
    if ((ev->key() == Qt::Key_Delete || ev->key() == Qt::Key_Backspace) && ev->modifiers() == Qt::NoModifier) {
        // copy to avoid bugs
        deleteLabel(QBitArray(labelSelection));
    } else {
        QLabel::keyPressEvent(ev);
    }
}

// this function handles label widget click event
bool WorkArea::eventFilter(QObject *obj, QEvent *ev) {
    auto index = labelWidgets.indexOf((QPushButton*)obj);

    if (index < 0) {
        return false;
    }

    if (ev->type() == QEvent::MouseButtonPress) {
        auto mev = dynamic_cast<QMouseEvent*>(ev);

        if (mev->button() == Qt::LeftButton) {
            if (mev->modifiers() == Qt::NoModifier) {
                clearSelection(false);
                select(index);
            } else if (mev->modifiers() == Qt::ControlModifier) {
                toggle(index);
            } else {
                return false;
            }
        } else if (mev->button() == Qt::RightButton) {
            if (mev->modifiers() == Qt::NoModifier) {
                QBitArray d(labelWidgets.count());
                d.setBit(index);
                deleteLabel(d);
            } else if (mev->modifiers() == Qt::ControlModifier) {
                // maybe ctrl + right button can be used to delete all selected labels, just like backspace and delete
                return false;
            } else {
                return false;
            }
        } else {
            return false;
        }

        return true;
    } else if (ev->type() == QEvent::MouseButtonDblClick) {
        auto mev = dynamic_cast<QMouseEvent*>(ev);

        if (mev->button() == Qt::LeftButton && mev->modifiers() == Qt::NoModifier) {
            op->doubleClickLabel(index);
        } else {
            return false;
        }

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
    QObject::connect(op, SIGNAL(labelContentUpdated(int)), this, SLOT(onLabelContentUpdated(int)));
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

    for (int i = 0; i < newLabelCount; ++i) {
        updateToolTip(i);
    }

    setPreferredZoomLevel();
}

/*
void WorkArea::onLabelAppended() {
    clearSelection(false);
    labelSelection.resize(labelSelection.count() + 1);
}
 */

void WorkArea::onLabelDeleted(QBitArray deleted) {
    if (suppressExternalSignal) {
        return;
    }

    deleteLabel(deleted, false);
}

void WorkArea::onLabelContentUpdated(int index) {
    updateToolTip(index);
}

void WorkArea::onLabelSelectionUpdated(QBitArray selected) {
    if (suppressExternalSignal) {
        return;
    }

    setSelection(selected, false);
}
