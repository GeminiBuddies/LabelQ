#include <model/builtin/RealPage.h>

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
