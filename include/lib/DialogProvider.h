#pragma once

#include <QWidget>
#include <QString>
#include <QFileDialog>
#include <QMessageBox>

class DialogProvider {
    QWidget *parent;

    static QString specifiedOrDefault(const QString &dir);

public:
    explicit DialogProvider(QWidget *parent);

    QString openFolder(const QString &title = QString(), const QString &dir = QString());
    QString openFile(const QString &filter = QString(), const QString &title = QString(), const QString &dir = QString());
    QStringList openFiles(const QString &filter = QString(), const QString &title = QString(), const QString &dir = QString());
    QString saveFile(const QString &filter = QString(), const QString &title = QString(), const QString &dir = QString());

    QMessageBox::StandardButton askYesNoCancel(const QString& title = QString(), const QString& text = QString());
    void warning(const QString& title = QString(), const QString& text = QString());
};
