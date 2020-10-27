#pragma once

#include "Model.h"
#include "Label.h"

#include <QBitArray>
#include <QObject>
#include <QString>
#include <QPixmap>

class Page : public QObject {
    Q_OBJECT

private:
    friend class Project;

protected:
    QVector<Label> _labels;

public:
    virtual bool dirty() = 0;

    virtual QPixmap pixmap() = 0;

    virtual void addLabel(Label &&label);
    virtual void addLabel(const Label &label);
    virtual Label label(int at);
    virtual void removeLabel(int at);
    virtual int labelCount();


};
