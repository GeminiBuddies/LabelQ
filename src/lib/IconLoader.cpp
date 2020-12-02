#include <lib/IconLoader.h>

QIcon loadIconFromFile(const QString &path) {
    QIcon icon;
    icon.addFile(path);
    return icon;
}
