#pragma once

#include "Model.h"

#include <QString>

class Page {
private:
    friend class Project;

    QString _path;

    // unfortunately model classes aren't QObject so Q_PROPERTY is not safe here...
public:
    QString path() const;
    void setPath(const QString &value);
};
