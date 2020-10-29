#include <model/builtin/TutorialProject.h>

#include <model/builtin/InMemoryPage.h>

TutorialProject::TutorialProject() {
    pages.append(new InMemoryPage("page1", QPixmap(":/images/example.png")));
}

TutorialProject::~TutorialProject() { }

void TutorialProject::clear() {
    for (auto p: pages) {
        auto imp = dynamic_cast<InMemoryPage*>(p);

        if (imp != nullptr) {
            imp->clear();
        }
    }
}
