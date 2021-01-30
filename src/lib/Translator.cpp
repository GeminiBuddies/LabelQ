#include <lib/Translator.h>

#include <QDebug>
#include <QApplication>

void loadLanguage(const QString &language) {
    static QTranslator *t = nullptr;

    if (t != nullptr) {
        QApplication::removeTranslator(t);
        delete t;
    }

    t = new QTranslator();
    qDebug() << "loading language: " << language;
    t->load(QString("labelq_") + language);

    QApplication::installTranslator(t);
}
