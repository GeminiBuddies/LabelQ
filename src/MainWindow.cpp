#include <MainWindow.h>
#include "../ui/ui_MainWindow.h"

#include <Definitions.h>
#include <model/Project.h>
#include <controller/ProjectOperator.h>
#include <controller/PageOperator.h>

#include <QMessageBox>
#include <QtCore/QTranslator>
#include <QScroller>
#include <QDate>

#include <cassert>

using namespace std;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    QIcon icon;
    icon.addPixmap(QPixmap(":/icons/setting.svg").scaled(128, 128));
    this->setWindowIcon(icon);

    ui->setupUi(this);
    customUiSetup();

    project = nullptr;
    currentPage = nullptr;
    currentPageIndex = -1;

    dp = new DialogProvider(this);
    prop = new ProjectOperator(dp);
    op = new PageOperator();

    ui->translationEditArea->setPageOperator(op);
    ui->workArea->setPageOperator(op);

    replaceProject(nullptr);
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

    QObject::connect(ui->pageListEdit, SIGNAL(clicked()), this, SLOT(togglePageEditing()));
    QObject::connect(ui->pageListEditDone, SIGNAL(clicked()), this, SLOT(togglePageEditing()));
    QObject::connect(ui->pageListToTop, SIGNAL(clicked()), this, SLOT(toTop()));
    QObject::connect(ui->pageListToBottom, SIGNAL(clicked()), this, SLOT(toBottom()));

    disablePageEditing();

    QObject::connect(ui->pageList->model(), SIGNAL(rowsMoved(const QModelIndex &, int, int, const QModelIndex &, int)), this, SLOT(pageListReordered(const QModelIndex &, int, int, const QModelIndex &, int)));
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

    disablePageEditing();
    if (project == nullptr) {
        ui->pageList->setEnabled(false);
    } else {
        ui->pageList->setEnabled(true);

        auto pageCount = project->pageCount();
        for (int i = 0; i < pageCount; ++i) {
            ui->pageList->addItem(project->page(i)->name());
        }
    }

    ui->pageListEdit->setVisible(project != nullptr && project->canReorderPages());
    ui->pageListAdd->setEnabled(project != nullptr && project->canAddAndRemovePages());
    ui->pageListRemove->setEnabled(project != nullptr && project->canAddAndRemovePages());

    if (project != nullptr && project->pageCount() > 0) {
        ui->pageList->item(0)->setSelected(true);
        // this statement implicitly contains
        //   setCurrentPage(0);
    } else {
        setCurrentPage(-1);
    }

    return true;
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

QVector<int> MainWindow::getSelectedPages() {
    auto selection = ui->pageList->selectionModel()->selectedIndexes();
    QVector<int> selectedRows;
    for (auto &index: selection) {
        selectedRows.append(index.row());
    }

    return selectedRows;
}

QVector<int> MainWindow::getSortedSelectedPages() {
    auto selectedRows = getSelectedPages();
    sort(selectedRows.begin(), selectedRows.end());
    return selectedRows;
}

void MainWindow::togglePageEditing() {
    pageEditEnabled = !pageEditEnabled;

    ui->pageListEdit->setVisible(!pageEditEnabled);
    ui->pageListEditDone->setVisible(pageEditEnabled);
    ui->pageListAdd->setVisible(pageEditEnabled);
    ui->pageListRemove->setVisible(pageEditEnabled);
    ui->pageListToTop->setVisible(pageEditEnabled);
    ui->pageListToBottom->setVisible(pageEditEnabled);

    ui->pageList->setDragEnabled(pageEditEnabled);

    if (pageEditEnabled) {
        ui->pageList->setSelectionMode(QAbstractItemView::ExtendedSelection);
        ui->pageList->setDragDropMode(QAbstractItemView::InternalMove);
        ui->pageList->setDefaultDropAction(Qt::MoveAction);
    } else {
        ui->pageList->setSelectionMode(QAbstractItemView::SingleSelection);
        ui->pageList->setDragDropMode(QAbstractItemView::NoDragDrop);
        ui->pageList->setDefaultDropAction(Qt::IgnoreAction);

        // when turning off edit, unselect all but the first selection
        auto selection = ui->pageList->selectionModel()->selectedIndexes();
        if (selection.size() > 1) {
            ui->pageList->item(selection[0].row())->setSelected(true);
        }
    }
}

void MainWindow::disablePageEditing() {
    pageEditEnabled = true;
    togglePageEditing();
}

void MainWindow::toTop() {
    auto moved = 0;
    for (auto row: reversed(getSortedSelectedPages())) {
        row += moved;

        auto item = ui->pageList->takeItem(row);
        ui->pageList->insertItem(0, item);
        project->movePage(row, 0);

        item->setSelected(true);

        ++moved;
    }
}

void MainWindow::toBottom() {
    auto end = ui->pageList->count() - 1;

    auto moved = 0;
    for (auto row: getSortedSelectedPages()) {
        row -= moved;

        auto item = ui->pageList->takeItem(row);
        ui->pageList->insertItem(end, item);
        project->movePage(row, end);

        item->setSelected(true);

        ++moved;
    }
}

void MainWindow::pageListReordered(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row) {
    assert(start == end);
    assert(start != row);

    project->movePage(start, row > start ? row - 1 : row);
}

void MainWindow::pageListSelectionItemChanged() {
    auto selection = ui->pageList->selectionModel()->selectedIndexes();

    if (selection.length() != 1) {
        setCurrentPage(-1);
    } else {
        auto index = selection[0].row();
        setCurrentPage(index);
    }

    auto anySelected = selection.length() > 0;
    ui->pageListRemove->setEnabled(anySelected && project != nullptr && project->canAddAndRemovePages());
    ui->pageListToTop->setEnabled(anySelected);
    ui->pageListToBottom->setEnabled(anySelected);
}

void MainWindow::showTutorial() {
    replaceProject(Project::tutorial());
}
