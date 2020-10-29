#pragma once

#include "../Page.h"

class InMemoryPage : public Page {
private:
    QString nameStr;
    QPixmap pic;

public:
    InMemoryPage(const QString &name, const QPixmap &pixmap);

    bool dirty() override;
    QString name() override;
    QPixmap pixmap() override;

    void clear();
};
