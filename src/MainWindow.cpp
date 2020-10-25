#include <MainWindow.h>
#include "../ui/ui_MainWindow.h"

#include <QMessageBox>
#include <QtCore/QTranslator>
#include <QScroller>

using namespace std;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    customUiSetup();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::zoom(bool in) {
    auto delta = in ? 5 : -5;
    auto dest = percentage + delta;

    if (dest < 25 || dest > 200) return;

    percentage = dest;

    // hide it first to avoid flickering
    ui->workArea->hide();

    auto newSize = pic.size() * percentage / 100;
    ui->menuB->setTitle(QString("%1, %2").arg(newSize.width()).arg(newSize.height()));
    ui->workArea->setPixmap(pic.scaled(newSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

    // to fix an unknown bug that workArea->width/height() return wrong value when zooming in
    ui->workArea->setFixedSize(newSize);

    ui->workArea->show();

    adjustWorkAreaMargin();
}

void MainWindow::zoomIn() {
    zoom(true);
}

void MainWindow::zoomOut() {
    zoom(false);
}

void MainWindow::showEvent(QShowEvent *event) {
    QMainWindow::showEvent(event);

    adjustWorkAreaMargin();
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);

    adjustWorkAreaMargin();
}

void MainWindow::adjustWorkAreaMargin() {
    auto vpH = ui->scrollArea->viewport()->width();
    auto vpV = ui->scrollArea->viewport()->height();
    auto marginH = max({(vpH - ui->workArea->width() - 2) / 2, vpH / 12, 64 });
    auto marginV = max({(vpV - ui->workArea->height() - 2) / 2, vpV / 12, 64 });

    /* margin debug info
    ui->menuA->setTitle(
        QString("margin: %1, %2 ").arg(marginH).arg(marginV) +
        QString("vp: %1, %2 ").arg(vpH).arg(vpV) +
        QString("label: %1, %2 ").arg(ui->label->width()).arg(ui->label->height()) +
        QString("V: %1, %2 ").arg((vpH - ui->label->width() - 2) / 2).arg(vpH / 12) +
        QString("H: %1, %2 ").arg((vpV - ui->label->height() - 2) / 2).arg(vpV / 12)
    );
     */

    ui->scrollAreaLayout->setContentsMargins(marginH, marginV, marginH, marginV);
}

void MainWindow::customUiSetup() {
    ui->mainSplitter->setStretchFactor(0, 1);
    ui->mainSplitter->setStretchFactor(1, 0);

    pic = QPixmap(":/images/example.png");
    ui->workArea->setPixmap(pic);
    percentage = 100;
    // ui->label->setText("WHY?");

    QObject::connect(ui->mainSplitter, SIGNAL(splitterMoved(int, int)), this, SLOT(adjustWorkAreaMargin()));
    QObject::connect(ui->scrollArea, SIGNAL(zoomIn()), this, SLOT(zoomIn()));
    QObject::connect(ui->scrollArea, SIGNAL(zoomOut()), this, SLOT(zoomOut()));
    QObject::connect(ui->textTable, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(x(int, int)));

    QScroller::grabGesture(ui->scrollArea, QScroller::RightMouseButtonGesture);
    auto scroller = QScroller::scroller(ui->scrollArea);
    auto props = scroller->scrollerProperties();
    props.setScrollMetric(QScrollerProperties::HorizontalOvershootPolicy, QScrollerProperties::OvershootAlwaysOff);
    props.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy, QScrollerProperties::OvershootAlwaysOff);
    props.setScrollMetric(QScrollerProperties::DecelerationFactor, 2.0);
    scroller->setScrollerProperties(props);

    ui->textTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->textTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->textTable->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    ui->textTable->setRowCount(64);
    for (int i = 0; i < 64; ++i) {
        auto header = new QTableWidgetItem();
        auto content = new QTableWidgetItem();

        header->setText(QString::number(i));
        content->setText(QString::number(1ull << (unsigned long long)i));

        ui->textTable->setVerticalHeaderItem(i, header);
        ui->textTable->setItem(i, 0, content);
    }

    delete ui->textTable->takeItem(42, 0);
    delete ui->textTable->takeVerticalHeaderItem(42);
    ui->textTable->removeRow(42);
}

void MainWindow::x(int r, int c) {
    QMessageBox::information(this, "", QString("Row %1, Column %2").arg(r).arg(c));
}
