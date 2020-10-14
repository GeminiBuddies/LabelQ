#include "entrywindow.h"

#include <QApplication>
#include <QtCore/QTranslator>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    QTranslator translator;
    translator.load(QString("labelq_") + "en_US");

    QApplication::installTranslator(&translator);

    EntryWindow w;
    w.show();

    return a.exec();
}
