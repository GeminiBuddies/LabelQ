#pragma once

#include "../Page.h"

class InMemoryPage : public Page {
private:
    QString nameStr;
    QPixmap pic;

protected:
    void calcPicSize() override;

public:
    InMemoryPage(const QString &name, const QPixmap &pixmap);

    QString name() override;
    QPixmap pixmap() override;

    void preDisplay() override;
    void postHidden() override;

    bool isDirty() override;

    void clear();
};
