#include <controller/ProjectOperator.h>

#include <model/Project.h>
#include <model/builtin/RealProject.h>
#include <model/builtin/RealPage.h>

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

    return replaceProject(nullptr);
}

bool ProjectOperator::loadTutorialProject() {
    if (!ensureProjectSaved()) {
        return false;
    }

    return replaceProject(Project::tutorial());
}

// just an alias
bool ProjectOperator::closeProject() {
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
    if (proj == nullptr) {
        return false;
    }

    replaceProject(proj);

    return true;
}

void ProjectOperator::addPage() {
    assert(currentProject != nullptr && currentProject->canAddAndRemovePages());

    auto images = dp->openFiles(tr("mainWindow_imageFilterDesc") + " " + ImageFilter);

    if (images.length() <= 0) {
        return;
    }

    assert(false); // todo: implement
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
        dp->warning(tr("mainWindow_LabelQ"), tr("mainWindow_imagesAndProjectMustBeInTheSameDir"));
        return false;
    }

    auto proj = Project::createNew();
    assert(proj->canAddAndRemovePages());

    proj->setWorkDir(projDir.absolutePath());

    for (auto &image: images) {
        proj->addPage(new RealPage(image));
    }

    replaceProject(proj);

    return true;
}

bool ProjectOperator::replaceProject(Project *project) {
    auto oldProject = currentProject;

    currentProject = nullptr;
    currentPageIndex = -1;
    currentPage = nullptr;

    emit pageSelectionUpdated(currentPage);

    currentProject = project;

    emit projectReplaced();

    if (oldProject != nullptr && oldProject->needDispose()) {
        Project::dispose(oldProject);
    }

    return true;
}

bool ProjectOperator::ensureProjectSaved() {
    if (currentProject == nullptr) {
        return true;
    }

    if (currentProject->canSave() && currentProject->isDirty()) {
        auto result = dp->askYesNoCancel(tr("mainWindow_saveChangesTitle"), tr("mainWindow_saveChanges"));

        if (result == QMessageBox::Yes) {
            return saveProject();
        } else if (result == QMessageBox::No) {
            // do nothing here
        } else {
            return false;
        }
    }

    return true;
}

bool ProjectOperator::saveProject() {
    if (currentProject->filename().isEmpty()) {
        auto projDir = currentProject->workDir();
        auto projPath = dp->saveFile(tr("mainWindow_projectFilterDesc") + " " + ProjectFilter, tr("mainWindow_saveProject"), projDir);

        if (projPath.isEmpty()) {
            return false;
        }

        if (QFileInfo(projPath).absoluteDir() != projDir) {
            dp->warning(tr("mainWindow_LabelQ"), tr("mainWindow_imagesAndProjectMustBeInTheSameDir"));
            return saveProject();
        }

        currentProject->setFilename(projPath);
    }

    currentProject->save();
    return true;
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

