#include <model/Project.h>

#include <model/builtin/TutorialProject.h>
#include <model/builtin/RealProject.h>
#include <Definitions.h>

bool Project::isDirty() {
    for (auto p: pages) {
        if (p->isDirty()) return true;
    }

    return false;
}

void Project::markSaved() {
    for (auto p: pages) {
        p->markSaved();
    }
}

bool Project::canSave() {
    return false;
}

void Project::save() {
    never();
}

void Project::exportProject(const QString &path) {
    never();
}

bool Project::needDispose() {
    return false;
}

QString Project::workDir() {
    return QString();
}

void Project::setWorkDir(const QString &) {
    never();
}

QString Project::filename() {
    return QString();
}

void Project::setFilename(const QString &) {
    never();
}

Page *Project::page(int at) {
    return pages.at(at);
}

int Project::pageCount() {
    return pages.count();
}

bool Project::pageExists(Page* page) {
    return pages.contains(page);
}

bool Project::canReorderPages() {
    return true;
}

void Project::movePage(int from, int to) {
    pages.move(from, to);
}

bool Project::canAddAndRemovePages() {
    return true;
}

void Project::addPage(Page *page) {
    pages.append(page);
}

void Project::removePage(const QBitArray &removed) {
    for (int i = pages.count() - 1; i >= 0; --i) {
        if (removed.testBit(i)) {
            pages.remove(i);
        }
    }
}

Project *Project::fromFile(const QString &path) {
    return RealProject::fromFile(path);
}

Project *Project::tutorial() {
    static TutorialProject tutorialProject;
    tutorialProject.clear();
    return &tutorialProject;
}

Project *Project::createNew() {
    return new RealProject();
}

void Project::dispose(Project *proj) {
    delete proj;
}
