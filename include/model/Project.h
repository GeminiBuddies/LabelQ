#pragma once

#include "Model.h"
#include "Page.h"

#include <QVector>

class Project {
protected:
    QVector<Page*> pages;

public:
    virtual ~Project();

    virtual bool dirty();
    virtual void save();

    virtual Page* page(int at);
    virtual int pageCount();
    virtual bool pageExists(Page *page);

    virtual bool canModifyPages();
    virtual bool canSave();
    virtual bool needDelete();

public:
    static Project* fromFile(const QString &path);
    static Project* tutorial();
    static Project* createNew();
};
