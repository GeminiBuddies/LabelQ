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

    // these functions returns false (reject action) if current project is not closed.
    bool openProject();
    bool loadTutorialProject();

signals:
    void projectReplaced();
};
