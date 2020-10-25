#include <MainWindow.h>

#include <QApplication>
#include <QtCore/QTranslator>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    QTranslator translator;
    // translator.load(QString("labelq_") + "en_US");
    translator.load(QString("labelq_") + "zh_CN");

    QApplication::installTranslator(&translator);

    MainWindow w;
    w.show();

    return a.exec();
}
