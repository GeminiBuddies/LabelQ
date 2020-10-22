#include <model/Page.h>

QString Page::path() const {
    return _path;
}

void Page::setPath(const QString &value) {
    _path = value;
}
