#include <entrywindow.h>
#include "../ui/ui_entrywindow.h"

#include <QtCore/QTranslator>

EntryWindow::EntryWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::EntryWindow)
{
    ui->setupUi(this);

    QObject::connect(ui->buttonEvil, SIGNAL(clicked()), this, SLOT(changeLanguage()));
    QObject::connect(ui->menuAItem1, SIGNAL(triggered()), this, SLOT(appendText()));
}

EntryWindow::~EntryWindow() {
    delete ui;
}

void EntryWindow::changeLanguage() {
    QTranslator translator;
    translator.load(QString("labelq_") + "zh_CN");
    QApplication::installTranslator(&translator);
    ui->retranslateUi(this);
}

void EntryWindow::appendText() {
    ui->textBrowser->append(tr("text_to_append"));
}
