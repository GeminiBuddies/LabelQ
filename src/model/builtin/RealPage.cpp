#include <model/builtin/RealPage.h>
#include <Definitions.h>

#include <QJsonArray>
#include <QDir>


void RealPage::calcPicSize() {
    preDisplay();
    postHidden();
}

RealPage::RealPage(const QString &path) : imageFile(path) { }

QString RealPage::name() {
    return imageFile.fileName();
}

QPixmap RealPage::pixmap() {
    return image;
}

void RealPage::preDisplay() {
    image.load(imageFile.absoluteFilePath());

    picWidth = image.width();
    picHeight = image.height();
}

void RealPage::postHidden() {
    image = QPixmap(0, 0);
}

RealPage *RealPage::fromJsonObject(const QJsonObject &json, const QString &baseDir) {
    if (!json.contains(LABELQ_PROJ_KEY_FILE) || !json.contains(LABELQ_PROJ_KEY_LABELS)) {
        return nullptr;
    }

    auto filename = json[LABELQ_PROJ_KEY_FILE];
    auto labels = json[LABELQ_PROJ_KEY_LABELS];

    if (!filename.isString() || !labels.isArray()) {
        return nullptr;
    }

    auto page = new RealPage(QDir(baseDir).filePath(filename.toString()));

    for (const auto &l: labels.toArray()) {
        if (!l.isObject()) {
            continue;
        }

        auto label = l.toObject();

        if (!label.contains(LABELQ_PROJ_KEY_X) || !label.contains(LABELQ_PROJ_KEY_Y) || !label.contains(LABELQ_PROJ_KEY_T)) {
            continue;
        }

        auto x = label[LABELQ_PROJ_KEY_X];
        auto y = label[LABELQ_PROJ_KEY_Y];
        auto t = label[LABELQ_PROJ_KEY_T];

        if (!x.isDouble() || !y.isDouble() || !t.isString()) {
            continue;
        }

        page->addLabel(Label(QPoint(x.toInt(), y.toInt()), t.toString()));
    }

    return page;
}

QJsonObject RealPage::toJsonObject() {
    QJsonObject obj;
    QJsonArray labelsArr;

    obj[LABELQ_PROJ_KEY_FILE] = imageFile.fileName();

    for (const auto &label: labels) {
        labelsArr.append(QJsonObject{
            { LABELQ_PROJ_KEY_X, label.position.x() },
            { LABELQ_PROJ_KEY_Y, label.position.y() },
            { LABELQ_PROJ_KEY_T, label.translation }
        });
    }

    obj[LABELQ_PROJ_KEY_LABELS] = labelsArr;

    return obj;
}
