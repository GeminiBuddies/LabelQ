#include <MainWindow.h>
#include <Definitions.h>
#include <lib/Translator.h>

#include <QDebug>
#include <QApplication>

#ifdef LANGUAGES
# define LANGUAGES_STR STR(LANGUAGES)
#else
# define LANGUAGES_STR ""
#endif

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    QString languagesStr(LANGUAGES_STR);
    auto languages = languagesStr.split('|', Qt::SkipEmptyParts);

    if (languages.length() > 0) {
        qDebug() << QString("%1 languages found").arg(languages.length());
        qDebug() << languages;
    } else {
        qDebug() << "No language found";
    }

    MainWindow w;
    w.setLanguages(languages);
    w.show();

    return QApplication::exec();
}
