#include <mainwindow.h>
#include "../ui/ui_mainwindow.h"

#include <QtCore/QTranslator>
#include <QScroller>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    customUiSetup();
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

}

void MainWindow::customUiSetup() {
    ui->mainSplitter->setStretchFactor(0, 1);
    ui->mainSplitter->setStretchFactor(1, 0);
    ui->label->setPixmap(QPixmap(":/images/example.png"));

    QScroller::grabGesture(ui->scrollArea, QScroller::RightMouseButtonGesture);
    auto scroller = QScroller::scroller(ui->scrollArea);
    auto props = scroller->scrollerProperties();
    props.setScrollMetric(QScrollerProperties::HorizontalOvershootPolicy, QScrollerProperties::OvershootAlwaysOff);
    props.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy, QScrollerProperties::OvershootAlwaysOff);
    scroller->setScrollerProperties(props);
}
