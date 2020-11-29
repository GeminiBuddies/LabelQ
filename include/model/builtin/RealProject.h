#pragma once

#include "../Project.h"

class RealProject : public Project {
protected:
    QString projFilePath;
    QString projDir;

    RealProject() = default;

public:
    bool canSave() override;
    void save() override;
    bool needDelete() override;

    static RealProject* fromFile(const QString &path);
    static void toFile(const QString &path, RealProject *proj);
};
