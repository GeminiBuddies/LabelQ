#include <MainWindow.h>
#include "../ui/ui_MainWindow.h"

#include <Definitions.h>
#include <model/Project.h>
#include <model/PageOperator.h>

#include <QMessageBox>
#include <QtCore/QTranslator>
#include <QScroller>
#include <QDate>

using namespace std;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    customUiSetup();

    project = nullptr;
    currentPage = nullptr;
    currentPageIndex = -1;
    op = new PageOperator();

    ui->translationEditArea->setPageOperator(op);
    ui->workArea->setPageOperator(op);

    replaceProject(nullptr);
}

MainWindow::~MainWindow() {
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
    QObject::connect(ui->workArea, SIGNAL(sizeChanged()), this, SLOT(adjustWorkAreaMargin()));

    QScroller::grabGesture(ui->scrollArea, QScroller::RightMouseButtonGesture);
    auto scroller = QScroller::scroller(ui->scrollArea);
    auto props = scroller->scrollerProperties();
    props.setScrollMetric(QScrollerProperties::HorizontalOvershootPolicy, QScrollerProperties::OvershootAlwaysOff);
    props.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy, QScrollerProperties::OvershootAlwaysOff);
    props.setScrollMetric(QScrollerProperties::DecelerationFactor, 2.0);
    scroller->setScrollerProperties(props);

    ui->workArea->setContainerWidget(ui->scrollArea);

    QObject::connect(ui->actionShowTutorial, SIGNAL(triggered()), this, SLOT(showTutorial()));
    QObject::connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(showAboutMessage()));
    QObject::connect(ui->actionAboutQt, SIGNAL(triggered()), this, SLOT(showAboutQtMessage()));
    QObject::connect(ui->pageList, SIGNAL(itemSelectionChanged()), this, SLOT(pageListSelectionItemChanged()));
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

void MainWindow::x(int r, int c) {
    QMessageBox::information(this, "", QString("Row %1, Column %2").arg(r).arg(c));
}

bool MainWindow::replaceProject(Project *newProject) {
    // close the old project
    auto oldProject = project;
    if (oldProject != nullptr) {
        if (oldProject->canSave() && oldProject->dirty()) {
            auto result = QMessageBox::question(this, tr("mainWindow_confirmExitTitle"), tr("mainWindow_confirmExitContent"), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

            if (result == QMessageBox::Yes) {
                oldProject->save();
            } else if (result == QMessageBox::No) {
                // do nothing here
            } else {
                return false;
            }
        }

        int pageCount = oldProject->pageCount();
        for (int i = pageCount - 1; i >= 0; --i) {
            delete ui->pageList->takeItem(i);
        }

        if (oldProject->needDelete()) {
            delete oldProject;
        }
    }

    // get the new one
    project = newProject;

    // set menuItems
    if (project == nullptr || !project->canSave()) {
        ui->actionSave->setEnabled(false);
        ui->actionSaveAs->setEnabled(false);
    } else {
        ui->actionSave->setEnabled(true);
        ui->actionSaveAs->setEnabled(true);
    }

    if (project == nullptr) {
        ui->pageList->setEnabled(false);
    } else {
        ui->pageList->setEnabled(true);

        auto pageCount = project->pageCount();
        for (int i = 0; i < pageCount; ++i) {
            ui->pageList->addItem(project->page(i)->name());
        }
    }

    if (project != nullptr && project->pageCount() > 0) {
        ui->pageList->item(0)->setSelected(true);
        // this statement contains
        //   setCurrentPage(0);
    } else {
        setCurrentPage(-1);
    }

    return true;
}

void MainWindow::pageListSelectionItemChanged() {
    auto selection = ui->pageList->selectionModel()->selectedIndexes();

    if (selection.length() == 0) {
        setCurrentPage(-1);
    } else {
        auto index = selection[0].row();
        setCurrentPage(index);
    }
}

void MainWindow::showTutorial() {
    replaceProject(Project::tutorial());
}

void MainWindow::setCurrentPage(int index) {
    if (project == nullptr || index < 0 || index >= project->pageCount()) {
        currentPage = nullptr;
        currentPageIndex = -1;
    } else {
        currentPage = project->page(index);
        currentPageIndex = index;
    }

    op->setPage(currentPage);
}
