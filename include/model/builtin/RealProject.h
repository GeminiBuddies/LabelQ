#pragma once

#include "../Project.h"

class RealProject : public Project {
private:
    friend class Project;

protected:
    QString projFilePath;
    QString projDir;

    RealProject() = default;

public:
    ~RealProject() override;

    bool isDirty() override;

    bool canSave() override;
    void save() override;
    bool needDispose() override;
    QString workDir() override;
    void setWorkDir(const QString &dir) override;
    QString filename() override;
    void setFilename(const QString &filename) override;

    static RealProject* fromFile(const QString &path);
    static void toFile(const QString &path, RealProject *proj);
};
