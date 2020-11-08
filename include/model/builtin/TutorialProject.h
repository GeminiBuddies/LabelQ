#pragma once

#include "../Project.h"

class TutorialProject : public Project {
    QVector<Page*> originalPages;

public:
    TutorialProject();
    ~TutorialProject() override;

    void clear();
};
