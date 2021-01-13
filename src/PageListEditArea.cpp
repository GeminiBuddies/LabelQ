#include <PageListEditArea.h>

#include <lib/IconLoader.h>
#include <model/Project.h>
#include <Definitions.h>

using namespace std;

PageListEditArea::PageListEditArea(QWidget *parent) : QWidget(parent) {
    pageListLayout = new QVBoxLayout(this);
    // pageListLayout->setSpacing(0);
    pageListLayout->setContentsMargins(0, 0, 0, 0);

    pageListEditorLayout = new QHBoxLayout();

    QSizePolicy sp(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    sp.setHorizontalStretch(0);
    sp.setVerticalStretch(0);

    pageListEdit = new QToolButton(this);
    pageListEdit->setSizePolicy(sp);
    pageListEdit->setIcon(loadIconFromFile(":/icons/setting.svg"));


    pageListEditDone = new QToolButton(this);
    pageListEditDone->setSizePolicy(sp);
    pageListEditDone->setIcon(loadIconFromFile(":/icons/ok.svg"));


    pageListAdd = new QToolButton(this);
    pageListAdd->setSizePolicy(sp);
    pageListAdd->setIcon(loadIconFromFile(":/icons/plus.svg"));


    pageListRemove = new QToolButton(this);
    pageListRemove->setSizePolicy(sp);
    pageListRemove->setIcon(loadIconFromFile(":/icons/minus.svg"));


    pageListToTop = new QToolButton(this);
    pageListToTop->setSizePolicy(sp);
    pageListToTop->setIcon(loadIconFromFile(":/icons/to_top.svg"));


    pageListToBottom = new QToolButton(this);
    pageListToBottom->setSizePolicy(sp);
    pageListToBottom->setIcon(loadIconFromFile(":/icons/to_bottom.svg"));

    pageListEditorLayout->addWidget(pageListEdit);
    pageListEditorLayout->addWidget(pageListEditDone);
    pageListEditorLayout->addWidget(pageListAdd);
    pageListEditorLayout->addWidget(pageListRemove);
    pageListEditorLayout->addWidget(pageListToTop);
    pageListEditorLayout->addWidget(pageListToBottom);

    pageListLayout->addLayout(pageListEditorLayout);

    pageList = new QListWidget(this);
    pageList->setEditTriggers(QAbstractItemView::NoEditTriggers);

    pageListLayout->addWidget(pageList);


    QObject::connect(pageListEdit, SIGNAL(clicked()), this, SLOT(togglePageEditing()));
    QObject::connect(pageListEditDone, SIGNAL(clicked()), this, SLOT(togglePageEditing()));
    QObject::connect(pageListToTop, SIGNAL(clicked()), this, SLOT(toTop()));
    QObject::connect(pageListToBottom, SIGNAL(clicked()), this, SLOT(toBottom()));

    QObject::connect(pageList, SIGNAL(itemSelectionChanged()), this, SLOT(pageListSelectionItemChanged()));
    QObject::connect(pageList->model(), &QAbstractItemModel::rowsMoved, [this](const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row) {
        assert(start == end);
        assert(start != row);

        op->project()->movePage(start, row > start ? row - 1 : row);
    });

    op = nullptr;
    disablePageEditing();

    suppressExternalSignal = false;
    // useless, just to suppress linter warning
    pageEditEnabled = false;
}

QVector<int> PageListEditArea::getSelectedPages() {
    auto selection = pageList->selectionModel()->selectedIndexes();
    QVector<int> selectedRows;
    for (auto &index: selection) {
        selectedRows.append(index.row());
    }

    return selectedRows;
}

QVector<int> PageListEditArea::getSortedSelectedPages() {
    auto selectedRows = getSelectedPages();
    sort(selectedRows.begin(), selectedRows.end());
    return selectedRows;
}

void PageListEditArea::projectReplaced() {
    auto project = op->project();

    int pageCount = pageList->count();
    for (int i = pageCount - 1; i >= 0; --i) {
        delete pageList->takeItem(i);
    }

    disablePageEditing();
    if (project == nullptr) {
        pageList->setEnabled(false);
    } else {
        pageList->setEnabled(true);

        pageCount = project->pageCount();
        for (int i = 0; i < pageCount; ++i) {
            pageList->addItem(project->page(i)->name());
        }
    }

    pageListEdit->setVisible(project != nullptr && project->canReorderPages());
    pageListAdd->setEnabled(project != nullptr && project->canAddAndRemovePages());
    pageListRemove->setEnabled(project != nullptr && project->canAddAndRemovePages());

    if (project != nullptr && project->pageCount() > 0) {
        pageList->item(0)->setSelected(true);
    }
}

void PageListEditArea::togglePageEditing() {
    pageEditEnabled = !pageEditEnabled;

    pageListEdit->setVisible(!pageEditEnabled);
    pageListEditDone->setVisible(pageEditEnabled);
    pageListAdd->setVisible(pageEditEnabled);
    pageListRemove->setVisible(pageEditEnabled);
    pageListToTop->setVisible(pageEditEnabled);
    pageListToBottom->setVisible(pageEditEnabled);

    pageList->setDragEnabled(pageEditEnabled);

    if (pageEditEnabled) {
        pageList->setSelectionMode(QAbstractItemView::ExtendedSelection);
        pageList->setDragDropMode(QAbstractItemView::InternalMove);
        pageList->setDefaultDropAction(Qt::MoveAction);
    } else {
        pageList->setSelectionMode(QAbstractItemView::SingleSelection);
        pageList->setDragDropMode(QAbstractItemView::NoDragDrop);
        pageList->setDefaultDropAction(Qt::IgnoreAction);

        // when turning off edit, unselect all but the first selection
        auto selection = pageList->selectionModel()->selectedIndexes();
        if (selection.size() > 1) {
            pageList->item(selection[0].row())->setSelected(true);
        }
    }
}

void PageListEditArea::disablePageEditing() {
    pageEditEnabled = true;
    togglePageEditing();
}

void PageListEditArea::toTop() {
    auto moved = 0;
    auto project = op->project();
    for (auto row: reversed(getSortedSelectedPages())) {
        row += moved;

        auto item = pageList->takeItem(row);
        pageList->insertItem(0, item);
        project->movePage(row, 0);

        item->setSelected(true);

        ++moved;
    }
}

void PageListEditArea::toBottom() {
    auto end = pageList->count() - 1;

    auto moved = 0;
    auto project = op->project();
    for (auto row: getSortedSelectedPages()) {
        row -= moved;

        auto item = pageList->takeItem(row);
        pageList->insertItem(end, item);
        project->movePage(row, end);

        item->setSelected(true);

        ++moved;
    }
}

void PageListEditArea::pageListSelectionItemChanged() {
    auto selection = pageList->selectionModel()->selectedIndexes();

    suppressExternalSignal = true;
    if (selection.length() != 1) {
        op->setPageSelection(-1);
    } else {
        auto index = selection[0].row();
        op->setPageSelection(index);
    }
    suppressExternalSignal = false;

    auto anySelected = selection.length() > 0;
    pageListRemove->setEnabled(anySelected && op->project() != nullptr && op->project()->canAddAndRemovePages());
    pageListToTop->setEnabled(anySelected);
    pageListToBottom->setEnabled(anySelected);
}

void PageListEditArea::pageSelectionChanged() {
    if (suppressExternalSignal) {
        return;
    }

    int index = op->pageIndex();
    if (index >= 0) {
        pageList->item(index)->setSelected(true);
    }
}

void PageListEditArea::setProjectOperator(ProjectOperator *pOperator) {
    this->op = pOperator;

    QObject::connect(this->op, SIGNAL(projectReplaced()), this, SLOT(projectReplaced()));
    QObject::connect(this->op, SIGNAL(pageSelectionUpdated(Page*)), this, SLOT(pageSelectionChanged()));
}
