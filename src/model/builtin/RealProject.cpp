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
    markSaved();
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
    projFile.close();
    if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
        return nullptr;
    }

    auto obj = doc.object();
    if (!obj.contains(LABELQ_PROJ_KEY_PAGES)) {
        return nullptr;
    }

    auto pages = obj[LABELQ_PROJ_KEY_PAGES];
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

    proj->markSaved();

    return proj;
}

void RealProject::toFile(const QString &path, RealProject *proj) {
    QJsonObject projObj;
    QJsonArray pagesArr;

    for (const auto &page: proj->pages) {
        pagesArr.append(page->toJsonObject());
    }

    projObj[LABELQ_PROJ_KEY_PAGES] = pagesArr;

    QJsonDocument doc(projObj);
    QFile projFile(path);
    if (projFile.open(QIODevice::WriteOnly)) {
        projFile.write(doc.toJson());
        projFile.close();
    } else {
        not_implemented();
    }
}
