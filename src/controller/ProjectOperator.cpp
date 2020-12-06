#include <controller/ProjectOperator.h>

#include <model/Project.h>

ProjectOperator::ProjectOperator(DialogProvider *dp) {
    this->dp = dp;
    this->currentProject = nullptr;

    this->currentPage = nullptr;
    this->currentPageIndex = -1;
}

bool ProjectOperator::loadEmptyProject() {
    if (!ensureProjectSaved()) {
        return false;
    }

    closeProject();
    emit projectReplaced();

    return true;
}

bool ProjectOperator::close() {
    if (currentProject == nullptr) {
        return true;
    }

    return loadEmptyProject();
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

    if (images.length() <= 0) {
        return false;
    }

    auto projDir = QFileInfo(images[0]).absoluteDir();
    if (std::any_of(images.begin(), images.end(), [projDir](QString &p){ return QFileInfo(p).absoluteDir() != projDir; })) {
        dp->warning(tr("mainWindow_LabelQ"), tr("mainWindow_imagesMustBeInTheSameDir"));
        return false;
    }


    return true;
}

bool ProjectOperator::loadTutorialProject() {
    return loadProject(Project::tutorial());
}

bool ProjectOperator::loadProject(Project *project) {
    if (project == nullptr) {
        return false;
    }

    return false;
}

bool ProjectOperator::ensureProjectSaved() {
    if (currentProject == nullptr) {
        return true;
    }

    if (currentProject->canSave() && currentProject->isDirty()) {
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
    if (currentProject != nullptr && currentProject->needDelete()) {
        delete currentProject;
    }

    currentProject = nullptr;
}

void ProjectOperator::setPageSelection(int index) {
    if (currentProject == nullptr || index < 0 || index >= currentProject->pageCount()) {
        currentPageIndex = -1;
        currentPage = nullptr;
    } else {
        currentPageIndex = index;
        currentPage = currentProject->page(index);
    }

    emit pageSelectionUpdated(currentPage);
}

Project *ProjectOperator::project() {
    return currentProject;
}
