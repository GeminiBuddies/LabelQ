#include <WorkArea.h>

#include <model/PageOperator.h>

WorkArea::WorkArea(QWidget *parent) : QLabel(parent) {
    currentZoomLevelIndex = 0;
    op = nullptr;
}

Page *WorkArea::page() const {
    return _currentPage;
}

void WorkArea::setPage(Page *page) {
    _currentPage = page;
}

void WorkArea::setZoomLevelIndex(int newIndex) {
    if (this->isHidden()) {
        return;
    }

    if (newIndex < MinimumZoomLevelIndex || newIndex > MaximumZoomLevelIndex || newIndex == currentZoomLevelIndex) {
        return;
    }

    currentZoomLevelIndex = newIndex;

    // hide it first to avoid flickering
    this->hide();

    auto newSize = pic.size() * zoomLevel(currentZoomLevelIndex) / 100;
    this->setPixmap(pic.scaled(newSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

    // to fix an unknown bug that workArea->width/height() return wrong value when zooming in
    this->setFixedSize(newSize);

    this->show();

    emit sizeChanged();
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

    this->setZoomLevelIndex(0);
}

void WorkArea::onLabelAppended() {

}

void WorkArea::onLabelDeleted(QBitArray deleted) {

}

void WorkArea::onLabelSelectionUpdated(QBitArray selected) {

}
