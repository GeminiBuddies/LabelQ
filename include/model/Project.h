#pragma once

#include "./Model.h"
#include "./Page.h"

#include <QVector>

class Project {
protected:
    QVector<Page*> pages;

public:
    virtual ~Project() = default;

    // project life-cycle management
    virtual bool isDirty();
    virtual void markSaved();
    virtual bool canSave();
    virtual void save();
    virtual void exportProject(const QString &path);
    virtual bool needDispose();
    virtual QString workDir();
    virtual void setWorkDir(const QString &dir);
    virtual QString filename();
    virtual void setFilename(const QString &filename);

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
    static void dispose(Project *proj);
};
