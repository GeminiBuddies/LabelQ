#pragma once

#include <QWidget>
#include <QString>
#include <QFileDialog>
#include <QMessageBox>

class DialogProvider {
    QWidget *parent;

public:
    explicit DialogProvider(QWidget *parent);

    QString openFolder(const QString& caption = QString(), const QString& dir = QString());
    QString openFile();

    QMessageBox::StandardButton askYesNoCancel(const QString& title = QString(), const QString& text = QString());
};
