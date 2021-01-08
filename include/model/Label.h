#pragma once

#include "./Model.h"

#include <QPoint>
#include <QString>

struct Label {
    QPoint position;
    QString translation;

    Label() = default;
    Label(const Label &) = default;
    Label(Label &&label) noexcept;
    Label(const QPoint &position, const QString &translation);

    Label& operator=(const Label &) = default;
};
