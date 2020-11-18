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

    if (currentProject->canSave() && currentProject->dirty()) {
        auto result = dp->askYesNoCancel(tr("mainWindow_confirmExitTitle"), tr("mainWindow_confirmExitContent"));

        if (result == QMessageBox::Yes) {

        } else if (result == QMessageBox::No) {
            // do nothing here
        } else {
            return false;
        }
    }

    if (currentProject->needDelete()) {
        delete currentProject;
    }

    currentProject = nullptr;
    emit projectReplaced();

    return true;
}

bool ProjectOperator::openProject() {


    return false;
}

bool ProjectOperator::loadTutorialProject() {
    return false;
}
