#pragma once

#include <QPoint>
#include <QString>

struct Label {
    QPoint position;
    QString translation;

    Label() = default;
    Label(const QPoint &position, const QString &translation);
};
