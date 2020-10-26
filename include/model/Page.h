#pragma once

#include "Model.h"
#include "Label.h"

#include <QString>
#include <QPixmap>

class Page {
private:
    friend class Project;

protected:
    QVector<Label> _labels;

    // unfortunately model classes aren't QObject so Q_PROPERTY is not safe here...
public:
    virtual bool dirty() = 0;

    virtual QPixmap pixmap() = 0;

    virtual void addLabel(Label &&label);
    virtual void addLabel(const Label &label);
    virtual Label label(int at);
    virtual void removeLabel(int at);
    virtual int labelCount();
};
