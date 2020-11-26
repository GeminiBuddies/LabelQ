#include <lib/DialogProvider.h>

#include <QFileDialog>
#include <QStandardPaths>

QString DialogProvider::specifiedOrDefault(const QString &dir) {
    return dir.length() > 0 ? dir : QStandardPaths::writableLocation(QStandardPaths::HomeLocation) ;
}

DialogProvider::DialogProvider(QWidget *parent) : parent(parent) {}

QString DialogProvider::openFolder(const QString &title, const QString &dir) {
    return QFileDialog::getExistingDirectory(parent, title, specifiedOrDefault(dir));
}

QString DialogProvider::openFile(const QString &filter, const QString &title, const QString &dir) {
    return QFileDialog::getOpenFileName(parent, title, specifiedOrDefault(dir), filter);
}

QStringList DialogProvider::openFiles(const QString &filter, const QString &title, const QString &dir) {
    return QFileDialog::getOpenFileNames(parent, title, specifiedOrDefault(dir), filter);
}

QString DialogProvider::saveFile(const QString &filter, const QString &title, const QString &dir) {
    return QFileDialog::getSaveFileName(parent, title, specifiedOrDefault(dir), filter);
}

QMessageBox::StandardButton DialogProvider::askYesNoCancel(const QString &title, const QString& text) {
    return QMessageBox::question(parent, title, text, QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
}

void DialogProvider::warning(const QString &title, const QString& text) {
    QMessageBox::warning(parent, title, text);
}
