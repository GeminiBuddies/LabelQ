#pragma once

#include "../Project.h"

class TutorialProject : public Project {
    QVector<Page*> originalPages;

public:
    TutorialProject();
    ~TutorialProject() override;

    bool canAddAndRemovePages() override;

    void clear();
};
