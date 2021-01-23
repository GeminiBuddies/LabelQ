#include <model/Page.h>
#include <model/builtin/RealPage.h>
#include <Definitions.h>

void Page::calcPicSize() {
    if (picWidth >= 0 && picHeight >= 0) {
        return;
    }

    auto pic = pixmap();
    picWidth = pic.width();
    picHeight = pic.height();
}

int Page::width() {
    if (picWidth < 0) {
        calcPicSize();
    }

    return picWidth;
}

int Page::height() {
    if (picHeight < 0) {
        calcPicSize();
    }

    return picHeight;
}

bool Page::isDirty() {
    return dirty;
}

void Page::markSaved() {
    dirty = false;
}

QJsonObject Page::toJsonObject() {
    never();
}

void Page::addLabel(Label &&label) {
    dirty = true;
    labels.push_back(std::move(label));
}

void Page::addLabel(const Label &label) {
    dirty = true;
    labels.push_back(label);
}

Label Page::label(int at) {
    return labels.at(at);
}

void Page::removeLabel(int at) {
    dirty = true;
    labels.remove(at);
}

int Page::labelCount() {
    return labels.count();
}

void Page::updateLabelContent(int at, const QString &content) {
    dirty = true;
    labels[at].translation = content;
}

Page *Page::fromJson(const QJsonObject &json, const QString &baseDir) {
    return RealPage::fromJsonObject(json, baseDir);
}
