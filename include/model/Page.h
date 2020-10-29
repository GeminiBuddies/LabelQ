#pragma once

#include "Model.h"
#include "Label.h"

#include <QBitArray>
#include <QString>
#include <QPixmap>

class Page {
private:
    friend class Project;

protected:
    QVector<Label> labels;

public:
    virtual ~Page() = default;

    virtual bool dirty() = 0;

    virtual QString name() = 0;
    virtual QPixmap pixmap() = 0;

    virtual void addLabel(Label &&label);
    virtual void addLabel(const Label &label);
    virtual Label label(int at);
    virtual void removeLabel(int at);
    virtual int labelCount();

    virtual Label& operator[](int at);
};
