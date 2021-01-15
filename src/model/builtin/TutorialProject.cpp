#include <model/builtin/TutorialProject.h>

#include <model/builtin/InMemoryPage.h>

TutorialProject::TutorialProject() {
    originalPages.append(new InMemoryPage("page1", QPixmap(":/images/example.png")));
    originalPages.append(new InMemoryPage("page2", QPixmap(":/images/example.png")));
    originalPages.append(new InMemoryPage("page3", QPixmap(":/images/example.png")));
    originalPages.append(new InMemoryPage("page4", QPixmap(":/images/example.png")));
    originalPages.append(new InMemoryPage("page5", QPixmap(":/images/example.png")));

    clear();
}

TutorialProject::~TutorialProject() { }

bool TutorialProject::canAddAndRemovePages() {
    return false;
}

void TutorialProject::clear() {
    pages = originalPages;

    for (auto p: pages) {
        auto imp = dynamic_cast<InMemoryPage*>(p);

        if (imp != nullptr) {
            imp->clear();
        }
    }
}
