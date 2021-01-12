#pragma once

#include "./Controller.h"

#include <model/Model.h>
#include <lib/DialogProvider.h>

#include <QObject>
#include <QBitArray>

class ProjectOperator : public QObject {
    Q_OBJECT

private:
    DialogProvider *dp;
    Project *currentProject;

public:
    const QString ProjectFilter = "(*.lbq)";
    const QString ImageFilter = "(*.png *.xpm *.jpg)";

    explicit ProjectOperator(DialogProvider *dp);

    // these functions are public apis
    bool loadEmptyProject();
    bool loadTutorialProject();

    bool closeProject();
    bool newProject();
    bool openProject();
    bool saveProject();

    void addPage();

private:
    bool replaceProject(Project *project);

    // ensure the current project is saved or changes are explicitly discarded
    // return true if
    //   a) the current project has been saved. or
    //   b) all changes have been discarded. or
    //   c) no project open
    bool ensureProjectSaved();

signals:
    void projectReplaced();
    void pageSelectionUpdated(Page *currentPage);

private:
    Page *currentPage;
    int currentPageIndex;

public:
    void selectPrevPage();
    void selectNextPage();
    void setPageSelection(int index);

public:
    Project *project();
};
