#include <model/Page.h>

void Page::addLabel(Label &&label) {
    _labels.push_back(label);
}

void Page::addLabel(const Label &label) {
    _labels.push_back(label);
}

Label Page::label(int at) {
    return _labels.at(at);
}

void Page::removeLabel(int at) {
    _labels.remove(at);
}

int Page::labelCount() {
    return _labels.count();
}
