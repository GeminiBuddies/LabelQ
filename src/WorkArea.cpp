#include <WorkArea.h>

WorkArea::WorkArea(QWidget *parent) : QLabel(parent) {

}

Page *WorkArea::page() const {
    return _currentPage;
}

void WorkArea::setPage(Page *page) {
    _currentPage = page;
}

void WorkArea::zoom(bool in) {

}
