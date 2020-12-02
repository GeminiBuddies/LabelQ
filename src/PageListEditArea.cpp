#include <PageListEditArea.h>

#include <lib/IconLoader.h>

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

}
