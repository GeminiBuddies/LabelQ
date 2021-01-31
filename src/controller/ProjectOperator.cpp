#include <controller/ProjectOperator.h>

#include <model/Project.h>
#include <model/builtin/RealProject.h>
#include <model/builtin/RealPage.h>
#include <Definitions.h>

QStringList ProjectOperator::dialogSelectImages(const QString &dir) {
    return dp->openFiles(tr("ui_dialog_common_filterDesc_imageFile") + " " + ImageFilter, tr("ui_dialog_selectImages_title"), dir);
}

QString ProjectOperator::dialogOpenProject(const QString &dir) {
    return dp->openFile(tr("ui_dialog_common_filterDesc_projectFile") + " " + ProjectFilter, tr("ui_dialog_openProject_title"), dir);
}

QString ProjectOperator::dialogSaveProject(const QString &dir) {
    return dp->saveFile(tr("ui_dialog_common_filterDesc_projectFile") + " " + ProjectFilter, tr("ui_dialog_saveProject_title"), dir);
}

void ProjectOperator::alarmSameDirectory() {
    dp->warning(tr("ui_dialog_common_title"), tr("ui_alarm_imagesAndProjectMustBeInTheSameDir"));
}

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

    auto projPath = dialogOpenProject();

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

bool ProjectOperator::newProject() {
    if (!ensureProjectSaved()) {
        return false;
    }

    auto images = dialogSelectImages();

    if (images.length() <= 0) {
        return false;
    }

    auto projDir = QFileInfo(images[0]).absoluteDir();
    if (std::any_of(images.begin(), images.end(), [projDir](QString &p){ return QFileInfo(p).absoluteDir() != projDir; })) {
        alarmSameDirectory();
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
        auto result = dp->askYesNoCancel(tr("ui_dialog_saveChanges_title"), tr("ui_dialog_saveChanges_content"));

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
        auto projPath = dp->saveFile(tr("ui_dialog_common_filterDesc_projectFile") + " " + ProjectFilter, tr("mainWindow_saveProject"), projDir);

        if (projPath.isEmpty()) {
            return false;
        }

        if (QFileInfo(projPath).absoluteDir() != projDir) {
            dp->warning(tr("ui_dialog_common_title"), tr("ui_alarm_imagesAndProjectMustBeInTheSameDir"));
            return saveProject();
        }

        currentProject->setFilename(projPath);
    }

    currentProject->save();
    return true;
}

bool ProjectOperator::exportLabelPlusProject() {
    assert(currentProject != nullptr && currentProject->canSave());

    auto projDir = currentProject->workDir();
    auto exportPath = dp->saveFile(tr("mainWindow_labelPlusProjectFilterDesc") + " " + LabelPlusProjectFilter, tr("mainWindow_exportProject"), projDir);

    if (QFileInfo(exportPath).absoluteDir() != projDir) {
        // TODO: replace it with a better warning message
        alarmSameDirectory();
        return false;
    }

    currentProject->exportProject(exportPath);
    return true;
}

void ProjectOperator::selectPrevPage() {
    if (currentProject == nullptr) {
        return;
    }

    if (currentPageIndex > 0 && currentPageIndex <= currentProject->pageCount() - 1) {
        setPageSelection(currentPageIndex - 1);
    }
}

void ProjectOperator::selectNextPage() {
    if (currentProject == nullptr) {
        return;
    }

    if (currentPageIndex >= 0 && currentPageIndex < currentProject->pageCount() - 1) {
        setPageSelection(currentPageIndex + 1);
    }
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

void ProjectOperator::movePage(int from, int to) {
    assert(currentProject != nullptr);

    currentProject->movePage(from, to);

    if (currentPageIndex == from) {
        currentPageIndex = to;
    } else {
        auto l = std::min(from, to);
        auto u = std::max(from, to);

        if (l <= currentPageIndex && currentPageIndex <= u) {
            if (from <= to) {
                currentPageIndex--;
            } else {
                currentPageIndex++;
            }
        }
    }

    emit pageListUpdated();
}

void ProjectOperator::addPage() {
    assert(currentProject != nullptr && currentProject->canAddAndRemovePages());

    auto projDir = currentProject->workDir();
    auto images = dialogSelectImages(projDir);

    if (images.length() <= 0) {
        return;
    }

    if (std::any_of(images.begin(), images.end(), [projDir](QString &p){ return QFileInfo(p).absoluteDir() != projDir; })) {
        alarmSameDirectory();
        return;
    }

    auto oldPageCount = currentProject->pageCount();

    for (auto &image: images) {
        currentProject->addPage(new RealPage(image));
    }

    emit pageListUpdated();

    setPageSelection(oldPageCount);
}

void ProjectOperator::removePage(const QBitArray &removed) {
    assert(currentProject != nullptr && currentProject->canAddAndRemovePages());

    currentProject->removePage(removed);

    // TODO: replace it with a better choice
    setPageSelection(-1);

    emit pageListUpdated();
}

Project *ProjectOperator::project() {
    return currentProject;
}

Page *ProjectOperator::page() {
    return currentPage;
}

int ProjectOperator::pageIndex() {
    return currentPageIndex;
}
