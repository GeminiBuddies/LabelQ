#include <mainwindow.h>
#include "../ui/ui_mainwindow.h"

#include <QtCore/QTranslator>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QObject::connect(ui->buttonEvil, SIGNAL(clicked()), this, SLOT(changeLanguage()));
    QObject::connect(ui->menuAItem1, SIGNAL(triggered()), this, SLOT(appendText()));
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::changeLanguage() {
    QTranslator translator;
    translator.load(QString("labelq_") + "zh_CN");
    QApplication::installTranslator(&translator);
    ui->retranslateUi(this);
}

void MainWindow::appendText() {
    ui->textBrowser->append(tr("text_to_append"));
}
