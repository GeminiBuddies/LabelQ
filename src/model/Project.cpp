#include <model/Project.h>

#include <model/builtin/TutorialProject.h>
#include <model/builtin/RealProject.h>

Project::~Project() {
    for (auto p: pages) {
        delete p;
    }
}

bool Project::isDirty() {
    for (auto p: pages) {
        if (p->isDirty()) return true;
    }

    return false;
}

bool Project::canSave() {
    return false;
}

void Project::save() {

}

bool Project::needDelete() {
    return false;
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
    return false;
}

void Project::addPage(Page *page) {
    pages.append(page);
}

void Project::removePage(const QBitArray &removed) {

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
    static Project emptyProject;
    return &emptyProject;
}
