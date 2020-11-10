#include <model/Project.h>

#include <model/builtin/TutorialProject.h>

Project::~Project() {
    for (auto p: pages) {
        delete p;
    }
}

bool Project::dirty() {
    for (auto p: pages) {
        if (p->dirty()) return true;
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

void Project::addPage(const QString &image) { }

void Project::removePage(const QBitArray &removed) { }

Project *Project::fromFile(const QString &path) {
    return nullptr;
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
