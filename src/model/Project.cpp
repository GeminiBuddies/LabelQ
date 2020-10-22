#include <model/Project.h>

Project::Project() {

}

Project::~Project() {
    for (auto p: _pages) {
        delete p;
    }
}

Page* Project::addPage(const QString &imagePath) {
    return nullptr;
}
