#include <model/builtin/RealProject.h>

#include <Definitions.h>

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

RealProject::~RealProject() {
    for (auto page: pages) {
        delete page;
    }
}

bool RealProject::isDirty() {
    if (projFilePath.isEmpty()) {
        return true;
    }

    return Project::isDirty();
}

bool RealProject::canSave() {
    return true;
}

void RealProject::save() {
    RealProject::toFile(projFilePath, this);
}

bool RealProject::needDispose() {
    return true;
}

QString RealProject::workDir() {
    return projDir;
}

void RealProject::setWorkDir(const QString &dir) {
    projDir = dir;
}

QString RealProject::filename() {
    return projFilePath;
}

void RealProject::setFilename(const QString &filename) {
    projFilePath = filename;
}

RealProject *RealProject::fromFile(const QString &path) {
    QFile projFile(path);
    if (!projFile.open(QIODevice::ReadOnly)) {
        return nullptr;
    }

    auto *proj = new RealProject();
    proj->projFilePath = path;
    proj->projDir = QFileInfo(path).absoluteDir().absolutePath();

    QJsonParseError parseError{};
    auto doc = QJsonDocument::fromJson(projFile.readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
        return nullptr;
    }

    auto obj = doc.object();
    if (!obj.contains("pages")) {
        return nullptr;
    }

    auto pages = obj["pages"];
    if (!pages.isArray()) {
        return nullptr;
    }

    auto pageArray = pages.toArray();
    for (const auto &item: pageArray) {
        if (!item.isObject()) {
            continue;
        }

        auto page = Page::fromJson(item.toObject(), proj->projDir);

        if (page == nullptr) {
            continue;
        }

        proj->addPage(page);
    }

    return proj;
}

void RealProject::toFile(const QString &path, RealProject *proj) {
    not_implemented();
}
