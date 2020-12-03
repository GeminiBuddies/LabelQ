#include <PageListEditArea.h>

#include <lib/IconLoader.h>
#include <model/Project.h>

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

    QObject::connect(pageList, SIGNAL(itemSelectionChanged()), this, SLOT(pageListSelectionItemChanged()));

    op = nullptr;
    disablePageEditing();

    // useless, just to suppress linter warning
    pageEditEnabled = false;
}

bool PageListEditArea::replaceProject(Project *newProject) {
    return false;
}

QVector<int> PageListEditArea::getSelectedPages() {
    return QVector<int>();
}

QVector<int> PageListEditArea::getSortedSelectedPages() {
    return QVector<int>();
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

}

void PageListEditArea::toBottom() {

}

void PageListEditArea::pageListReordered(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row) {

}

void PageListEditArea::pageListSelectionItemChanged() {
    auto selection = pageList->selectionModel()->selectedIndexes();

    if (selection.length() != 1) {
        op->setPageSelection(-1);
    } else {
        auto index = selection[0].row();
        op->setPageSelection(index);
    }

    auto anySelected = selection.length() > 0;
    pageListRemove->setEnabled(anySelected && op->project() != nullptr && op->project()->canAddAndRemovePages());
    pageListToTop->setEnabled(anySelected);
    pageListToBottom->setEnabled(anySelected);
}
