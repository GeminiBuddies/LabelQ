#pragma once

#include "./Model.h"
#include "./Page.h"

#include <QVector>

class Project {
protected:
    QVector<Page*> pages;

public:
    virtual ~Project();

    // project life-cycle management
    virtual bool isDirty();
    virtual bool canSave();
    virtual void save();
    virtual bool needDelete();

    // page management
    // - get
    virtual Page* page(int at);
    virtual int pageCount();
    virtual bool pageExists(Page *page);

    // - reordering
    virtual bool canReorderPages();
    virtual void movePage(int from, int to);

    // - adding and removing
    virtual bool canAddAndRemovePages();
    virtual void addPage(Page *page);
    virtual void removePage(const QBitArray &removed);

public:
    static Project* fromFile(const QString &path);
    static Project* tutorial();
    static Project* createNew();
};
