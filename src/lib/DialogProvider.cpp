#include <lib/DialogProvider.h>

#include <QFileDialog>
#include <QStandardPaths>

DialogProvider::DialogProvider(QWidget *parent) : parent(parent) {}

QString DialogProvider::openFolder(const QString &caption, const QString &dir) {
    const QString &d = dir.length() > 0 ? dir : QStandardPaths::writableLocation(QStandardPaths::HomeLocation);

    return QFileDialog::getExistingDirectory(parent, caption, dir);
}

QString DialogProvider::openFile() {
    return "";
}

QMessageBox::StandardButton DialogProvider::askYesNoCancel(const QString &title, const QString& text) {
    return QMessageBox::question(parent, title, text, QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
}

