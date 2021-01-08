#include <model/builtin/RealPage.h>
#include <Definitions.h>

#include <QJsonArray>

RealPage::RealPage(const QString &path) : imageFile(path) { }

QString RealPage::name() {
    return imageFile.fileName();
}

QPixmap RealPage::pixmap() {
    return image;
}

void RealPage::preDisplay() {
    image.load(imageFile.absoluteFilePath());
}

void RealPage::postHidden() {
    image = QPixmap(0, 0);
}

RealPage *RealPage::fromJsonObject(const QJsonObject &json, const QString &baseDir) {
    not_implemented();
}

QJsonObject RealPage::toJsonObject() {
    QJsonObject obj;
    QJsonArray labelsArr;

    obj["file"] = imageFile.fileName();

    for (const auto &label: labels) {
        labelsArr.append(QJsonObject{
            { "x", label.position.x() },
            { "y", label.position.y() },
            { "t", label.translation }
        });
    }

    obj["labels"] = labelsArr;

    return obj;
}
