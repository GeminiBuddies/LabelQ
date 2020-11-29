#include <model/builtin/InMemoryPage.h>


InMemoryPage::InMemoryPage(const QString &name, const QPixmap &pixmap) {
    this->nameStr = name;
    this->pic = pixmap;
}

QString InMemoryPage::name() {
    return nameStr;
}

QPixmap InMemoryPage::pixmap() {
    return pic;
}

void InMemoryPage::clear() {
    labels.clear();
}

void InMemoryPage::preDisplay() { }

void InMemoryPage::postHidden() { }

bool InMemoryPage::isDirty() {
    return false;
}
