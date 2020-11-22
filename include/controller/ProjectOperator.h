#pragma once

#include "./Controller.h"

#include <model/Model.h>
#include <lib/DialogProvider.h>

#include <QObject>

class ProjectOperator : QObject {
    Q_OBJECT

private:
    DialogProvider *dp;
    Project *currentProject;

public:
    explicit ProjectOperator(DialogProvider *dp);

    // try close the current project. return false if this action get cancelled by user.
    bool close();
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
};
