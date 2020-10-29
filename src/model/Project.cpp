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

void Project::save() {

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

bool Project::canModifyPages() {
    return false;
}

bool Project::canSave() {
    return false;
}

bool Project::needDelete() {
    return false;
}

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
