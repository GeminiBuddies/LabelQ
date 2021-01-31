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

    QStringList dialogSelectImages(const QString &dir = QString());
    QString dialogOpenProject(const QString &dir = QString());
    QString dialogSaveProject(const QString &dir = QString());

    void alarmSameDirectory();

public:
    const QString ProjectFilter = "(*.lbq)";
    const QString LabelPlusProjectFilter = "(*.txt)";
    const QString ImageFilter = "(*.png *.xpm *.jpg)";

    explicit ProjectOperator(DialogProvider *dp);

    // these functions are public apis
    bool loadEmptyProject();
    bool loadTutorialProject();

    bool closeProject();
    bool newProject();
    bool openProject();
    bool saveProject();
    bool saveProjectAs();
    bool exportLabelPlusProject();

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
    void pageListUpdated();

private:
    Page *currentPage;
    int currentPageIndex;

public:
    void selectPrevPage();
    void selectNextPage();
    void setPageSelection(int index);

    void movePage(int from, int to);
    void addPage();
    void removePage(const QBitArray &removed);

public:
    Project* project();
    Page* page();
    int pageIndex();
};
