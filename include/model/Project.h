#pragma once

#include "Model.h"
#include "Page.h"

#include <QVector>

class Project {
private:
    QVector<Page*> _pages;

    Project();

public:
    ~Project();

    Page* addPage(const QString &imagePath);
    Page* getPage(int at);

public:
    static Project* fromFile(const QString &path);
    static Project* create();
};
