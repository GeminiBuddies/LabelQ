#include <model/builtin/RealProject.h>

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>

bool RealProject::canSave() {
    return true;
}

void RealProject::save() {
    RealProject::toFile(projFilePath, this);
}

bool RealProject::needDelete() {
    return true;
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
}

void RealProject::toFile(const QString &path, RealProject *proj) {

}
