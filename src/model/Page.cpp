#include <model/Page.h>

void Page::addLabel(Label &&label) {
    labels.push_back(label);
}

void Page::addLabel(const Label &label) {
    labels.push_back(label);
}

Label Page::label(int at) {
    return labels.at(at);
}

void Page::removeLabel(int at) {
    labels.remove(at);
}

int Page::labelCount() {
    return labels.count();
}

Label &Page::operator[](int at) {
    return labels[at];
}
