#pragma once

#include "./Controller.h"

#include <model/Model.h>
#include <lib/DialogProvider.h>

#include <QObject>
#include <QBitArray>

class ProjectOperator : QObject {
    Q_OBJECT

private:
    DialogProvider *dp;
    Project *currentProject;

public:
    const QString ProjectFilter = "(*.lbq)";
    const QString ImageFilter = "(*.png *.xpm *.jpg)";

    explicit ProjectOperator(DialogProvider *dp);

    // try close the current project. return false if this action get cancelled by user.
    bool close();
    bool newProject();
    bool openProject();
    bool loadTutorialProject();

private:
    bool loadProject(Project *project);

    // return true if
    //   a) the current project has been saved. or
    //   b) all changes have been discarded. or
    //   c) no project open
    bool ensureProjectSaved();
    void closeProject();

signals:
    void projectReplaced();

    void pageSelectionUpdated(const QBitArray &selected);
};
