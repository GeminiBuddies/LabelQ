#include <WorkArea.h>

#include <model/PageOperator.h>

#include <cmath>

WorkArea::WorkArea(QWidget *parent) : QLabel(parent) {
    currentZoomLevelIndex = 0;
    containerWidget = nullptr;
    op = nullptr;

    button = new QPushButton(this);
    button->setGeometry(100, 100, 40, 40);
    button->setStyleSheet("QPushButton { border: 2px solid gray; border-radius: 4px; }");
    button->setFont(QFont("DejaVu Sans Mono", 11));
    button->setText("42");
}

void WorkArea::setContainerWidget(QWidget *widget) {
    containerWidget = widget;
}

void WorkArea::setZoomLevelIndex(int newIndex, bool forced) {
    if (newIndex < MinimumZoomLevelIndex || newIndex > MaximumZoomLevelIndex || newIndex == currentZoomLevelIndex) {
        if (!forced) return;

        newIndex = std::max(MinimumZoomLevelIndex, newIndex);
        newIndex = std::min(MaximumZoomLevelIndex, newIndex);
    }

    currentZoomLevelIndex = newIndex;

    if (this->isHidden()) {
        return;
    }

    // hide it first to avoid flickering
    this->hide();

    auto ratio = zoomLevel(currentZoomLevelIndex) / 100.0;
    auto newSize = pic.size() * ratio;
    this->setPixmap(pic.scaled(newSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

    // to fix an unknown bug that workArea->width/height() return wrong value when zooming in
    this->setFixedSize(newSize);

    auto x = std::min(newSize.width(), newSize.height());
    button->setGeometry(QRect(QPoint(400 * ratio, 300 * ratio), QSize(x, x) * 0.04));

    this->show();

    emit sizeChanged();
}

void WorkArea::setPreferredZoomLevel() {
    // setZoomLevelIndex(0);

    if (this->isHidden()) {
        setZoomLevelIndex(0);
        return;
    }

    auto parentSize = containerWidget->size();
    auto preferredZoomVertical = 5.0 / 6.0 * parentSize.height() / pic.height();
    auto preferredZoomHorizontal = 5.0 / 6.0 * parentSize.width() / pic.width();
    auto preferredZoom = std::max(preferredZoomVertical, preferredZoomHorizontal);
    preferredZoom = std::floor(std::min(preferredZoom, zoomLevel(MaximumZoomLevelIndex) / 100.0) * 100);

    auto preferredZoomLevel = (int)preferredZoom;
    auto preferredIndex = std::lower_bound(ZoomLevels, ZoomLevels + ZoomLevelCount, preferredZoomLevel) - ZoomLevels + MinimumZoomLevelIndex;

    setZoomLevelIndex(preferredIndex, true);
}

void WorkArea::zoom(bool in) {
    setZoomLevelIndex(currentZoomLevelIndex + (in ? 1 : -1));
}

void WorkArea::zoomReset() {
    setZoomLevelIndex(0);
}

void WorkArea::setPageOperator(PageOperator *op) {
    if (this->op != nullptr) {
        return;
    }

    this->op = op;

    QObject::connect(op, SIGNAL(newPageSet()), this, SLOT(onNewPage()));
    QObject::connect(op, SIGNAL(labelAppended()), this, SLOT(onLabelAppended()));
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

    setPreferredZoomLevel();
}

void WorkArea::onLabelAppended() {

}

void WorkArea::onLabelDeleted(QBitArray deleted) {

}

void WorkArea::onLabelSelectionUpdated(QBitArray selected) {

}
