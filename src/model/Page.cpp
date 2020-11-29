#include <model/Page.h>

bool Page::isDirty() {
    return dirty;
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
    dirty = true;
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

Label &Page::operator[](int at) {
    return labels[at];
}
