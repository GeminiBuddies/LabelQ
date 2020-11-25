#include <controller/ProjectOperator.h>

#include <model/Project.h>

ProjectOperator::ProjectOperator(DialogProvider *dp) {
    this->dp = dp;
    this->currentProject = nullptr;
}

bool ProjectOperator::close() {
    if (currentProject == nullptr) {
        return true;
    }

    if (!ensureProjectSaved()) {
        return false;
    }

    closeProject();
    emit projectReplaced();

    return true;
}

bool ProjectOperator::openProject() {
    if (!ensureProjectSaved()) {
        return false;
    }

    auto projPath = dp->openFile(tr("mainWindow_projectFilterDesc") + " " + ProjectFilter);

    if (projPath.length() <= 0) {
        return false;
    }

    auto proj = Project::fromFile(projPath);

    return true;
}

bool ProjectOperator::newProject() {
    if (!ensureProjectSaved()) {
        return false;
    }

    auto images = dp->openFiles(tr("mainWindow_imageFilterDesc") + " " + ImageFilter);

    return true;
}

bool ProjectOperator::loadTutorialProject() {
    return loadProject(Project::tutorial());
}

bool ProjectOperator::loadProject(Project *project) {
    return false;
}

bool ProjectOperator::ensureProjectSaved() {
    if (currentProject == nullptr) {
        return true;
    }

    if (currentProject->canSave() && currentProject->dirty()) {
        auto result = dp->askYesNoCancel(tr("mainWindow_saveChangesTitle"), tr("mainWindow_saveChanges"));

        if (result == QMessageBox::Yes) {
            currentProject->save();
        } else if (result == QMessageBox::No) {
            // do nothing here
        } else {
            return false;
        }
    }

    return true;
}

void ProjectOperator::closeProject() {
    if (currentProject->needDelete()) {
        delete currentProject;
    }

    currentProject = nullptr;
}
