#include <MainWindow.h>
#include "../ui/ui_MainWindow.h"

#include <Definitions.h>
#include <model/Project.h>
#include <lib/IconLoader.h>
#include <controller/ProjectOperator.h>
#include <controller/PageOperator.h>

#include <QMessageBox>
#include <QtCore/QTranslator>
#include <QScroller>
#include <QDate>

#include <cassert>

using namespace std;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    this->setWindowIcon(loadIconFromFile(":/icons/setting.svg"));

    ui->setupUi(this);
    customUiSetup();

    dp = new DialogProvider(this);
    prop = new ProjectOperator(dp);
    op = new PageOperator();

    ui->pageListEditArea->setProjectOperator(prop);
    ui->translationEditArea->setPageOperator(op);
    ui->workArea->setPageOperator(op);

    connect(prop, &ProjectOperator::pageSelectionUpdated, [this](Page *currentPage) {
        this->op->setPage(currentPage);
    });

    connect(prop, &ProjectOperator::projectReplaced, [this]() {
        // get the new one
        auto project = this->prop->project();

        // set menuItems
        if (project == nullptr || !project->canSave()) {
            ui->actionSave->setEnabled(false);
            ui->actionSaveAs->setEnabled(false);
        } else {
            ui->actionSave->setEnabled(true);
            ui->actionSaveAs->setEnabled(true);
        }
    });

    prop->loadEmptyProject();
}

MainWindow::~MainWindow() {
    delete dp;
    delete prop;
    delete op;
    delete ui;
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

    // "-2" here is necessary
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

    QObject::connect(ui->mainSplitter, SIGNAL(splitterMoved(int, int)), this, SLOT(adjustWorkAreaMargin()));
    QObject::connect(ui->scrollArea, SIGNAL(zoom(bool)), ui->workArea, SLOT(zoom(bool)));
    QObject::connect(ui->scrollArea, SIGNAL(zoomReset()), ui->workArea, SLOT(zoomReset()));
    QObject::connect(ui->scrollArea, SIGNAL(prev()), this, SLOT(selectPrevPage()));
    QObject::connect(ui->scrollArea, SIGNAL(next()), this, SLOT(selectNextPage()));
    QObject::connect(ui->workArea, SIGNAL(sizeChanged()), this, SLOT(adjustWorkAreaMargin()));

    QScroller::grabGesture(ui->scrollArea, QScroller::RightMouseButtonGesture);
    auto scroller = QScroller::scroller(ui->scrollArea);
    auto props = scroller->scrollerProperties();
    props.setScrollMetric(QScrollerProperties::HorizontalOvershootPolicy, QScrollerProperties::OvershootAlwaysOff);
    props.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy, QScrollerProperties::OvershootAlwaysOff);
    props.setScrollMetric(QScrollerProperties::DecelerationFactor, 2.0);
    scroller->setScrollerProperties(props);

    ui->workArea->setContainerWidget(ui->scrollArea);

    QObject::connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(newProject()));
    QObject::connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(openProject()));
    QObject::connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(saveProject()));;
    QObject::connect(ui->actionSaveAs, SIGNAL(triggered()), this, SLOT(saveProjectAs()));;
    QObject::connect(ui->actionClose, SIGNAL(triggered()), this, SLOT(closeProject()));
    QObject::connect(ui->actionShowTutorial, SIGNAL(triggered()), this, SLOT(showTutorial()));
    QObject::connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(exitProgram()));
    QObject::connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(showAboutMessage()));
    QObject::connect(ui->actionAboutQt, SIGNAL(triggered()), this, SLOT(showAboutQtMessage()));
}

void MainWindow::selectPrevPage() {
    prop->selectPrevPage();
}

void MainWindow::selectNextPage() {
    prop->selectNextPage();
}

void MainWindow::newProject() {
    prop->newProject();
}

void MainWindow::openProject() {
    prop->openProject();
}

void MainWindow::saveProject() {
    prop->saveProject();
}

void MainWindow::saveProjectAs() {

}

void MainWindow::closeProject() {
    prop->closeProject();
}

void MainWindow::showTutorial() {
    prop->loadTutorialProject();
}

void MainWindow::exitProgram() {
    if (prop->closeProject()) {
        QApplication::exit(0);
    }
}

void MainWindow::showAboutMessage() {
    // args
    // %0: version
    // %1: startYear
    // %2: endYear

    auto content =
            tr("mainWindow_aboutContent")
            .arg(LABELQ_VERSION)
            .arg(2020)
            .arg(QDate::currentDate().year())
            ;
    QMessageBox::about(this, tr("mainWindow_aboutTitle"), content);
}

void MainWindow::showAboutQtMessage() {
    QMessageBox::aboutQt(this);
}
