#include <model/PageOperator.h>

#include <model/Page.h>

PageOperator::PageOperator() {
    currentPage = nullptr;
}

void PageOperator::setPage(Page *page) {
    currentPage = page;
    emit newPageSet();
}

Page* PageOperator::page() {
    return currentPage;
}

int PageOperator::appendLabel(const QPoint &position) {
    currentPage->addLabel(std::move(Label(position, "")));

    return currentPage->labelCount();
}

void PageOperator::deleteLabel(const QBitArray &deleted) {
    int len = currentPage->labelCount();

    for (int i = len - 1; i >= 0; --i) {
        if (deleted.testBit(i)) {
            currentPage->removeLabel(i);
        }
    }

    emit labelDeleted(deleted);
}

void PageOperator::setLabelContent(int index, const QString &content) {

}

void PageOperator::setSelection(const QBitArray &selected) {
    emit labelSelectionUpdated(selected);
}
