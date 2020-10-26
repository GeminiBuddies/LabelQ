#include <model/Project.h>

Project::~Project() {
    for (auto p: _pages) {
        delete p;
    }
}

bool Project::dirty() {
    for (auto p: _pages) {
        if (p->dirty()) return true;
    }

    return false;
}

void Project::save() {

}

Page *Project::page(int at) {
    return _pages.at(at);
}

int Project::pageCount() {
    return _pages.count();
}

bool Project::pageExists(Page* page) {
    return _pages.contains(page);
}

bool Project::canModifyPages() {
    return false;
}

bool Project::canSave() {
    return false;
}

Project *Project::fromFile(const QString &path) {
    return nullptr;
}

Project *Project::tutorial() {
    return nullptr;
}

Project *Project::empty() {
    static Project emptyProject;
    return &emptyProject;
}

class TutorialProject {

};
