#include <model/builtin/RealProject.h>

#include <Definitions.h>
#include <model/builtin/RealPage.h>

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <cmath>

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

void RealProject::exportProject(const QString &path) {
    toLabelPlus(path, this);
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
        projFile.write(doc.toJson(QJsonDocument::Compact));
        projFile.close();
    } else {
        not_implemented();
    }
}

void RealProject::toLabelPlus(const QString &path, RealProject *proj) {
    const QString pageHeader(">>>>>>>>[%1]<<<<<<<<\n");
    const QString labelHeader("----------------[%1]----------------[0.%2,0.%3,1]\n");

    QFile exportFile(path);
    if (!exportFile.open(QIODevice::WriteOnly)) {
        not_implemented();
    }

    // write headers
    exportFile.write("1,0\n");
    exportFile.write("-\n");
    exportFile.write("LabelQ\n");
    exportFile.write("-\n");
    exportFile.write("Exported from LabelQ\n");
    exportFile.write("\n");
    exportFile.write("\n");

    for (auto page: proj->pages) {
        exportFile.write(pageHeader.arg(page->name()).toUtf8());

        int w = page->width();
        int h = page->height();

        auto labelCount = page->labelCount();
        for (int l = 0; l < labelCount; ++l) {
            auto label = page->label(l);

            int x = label.position.x();
            int y = label.position.y();

            x = (int)lround(x * 1000.0 / w);
            y = (int)lround(y * 1000.0 / h);

            if (x >= 1000) {
                x = 999;
            }

            if (y >= 1000) {
                y = 999;
            }

            exportFile.write(labelHeader.arg(l + 1).arg(x, 3, 10, QChar('0')).arg(y, 3, 10, QChar('0')).toUtf8());
            exportFile.write(label.translation.toUtf8());
            exportFile.write("\n\n");
        }

        exportFile.write("\n");
    }

    exportFile.close();
}
