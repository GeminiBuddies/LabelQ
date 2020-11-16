#include <TranslationEditArea.h>

#include <model/Page.h>
#include <controller/PageOperator.h>

#include <QKeyEvent>
#include <QHeaderView>

QTableWidgetWithKeySignal::QTableWidgetWithKeySignal(QWidget *parent) : QTableWidget(parent) {}

void QTableWidgetWithKeySignal::keyPressEvent(QKeyEvent *event) {
    if ((event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) && event->modifiers() == Qt::NoModifier) {
        emit deleteKeyPressed();
    } else {
        QTableWidget::keyPressEvent(event);
    }
}

TranslationEditArea::TranslationEditArea(QWidget *parent) : QWidget(parent) {
    verticalLayout = new QVBoxLayout(this);
    verticalLayout->setSpacing(0);
    verticalLayout->setContentsMargins(0, 0, 0, 0);

    splitter = new QSplitter(this);
    splitter->setOrientation(Qt::Vertical);

    translationTable = new QTableWidgetWithKeySignal(splitter);

    translationTable->setColumnCount(2);
    translationTable->setHorizontalHeaderItem(0, new QTableWidgetItem(tr("mainWindow_textTable_textHeader")));
    translationTable->setHorizontalHeaderItem(1, new QTableWidgetItem(""));
    translationTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    translationTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    translationTable->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

    translationTable->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    translationTable->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    translationTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
    translationTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    translationTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    translationText = new QTextEdit(splitter);

    splitter->addWidget(translationTable);
    splitter->addWidget(translationText);

    verticalLayout->addWidget(splitter);

    QObject::connect(translationTable, SIGNAL(itemSelectionChanged()), this, SLOT(tableSelectionChanged()));
    QObject::connect(translationTable, SIGNAL(deleteKeyPressed()), this, SLOT(deleteKeyPressed()));
    QObject::connect(translationText, SIGNAL(textChanged()), this, SLOT(textEdited()));
    QObject::connect(translationTable, SIGNAL(itemDoubleClicked(QTableWidgetItem*)), this, SLOT(tableItemDoubleClicked(QTableWidgetItem*)));

    op = nullptr;

    suppressUIEvents = false;
    suppressExternalSignal = false;
    selectedLabel = -1;
}

TranslationEditArea::~TranslationEditArea() {}

void TranslationEditArea::setPageOperator(PageOperator *op) {
    if (this->op != nullptr) {
        return;
    }

    this->op = op;

    QObject::connect(op, SIGNAL(newPageSet()), this, SLOT(onNewPage()));
    QObject::connect(op, SIGNAL(labelAppended()), this, SLOT(onLabelAppended()));
    QObject::connect(op, SIGNAL(labelDeleted(QBitArray)), this, SLOT(onLabelDeleted(QBitArray)));
    QObject::connect(op, SIGNAL(labelContentUpdated(int)), this, SLOT(onLabelContentUpdated(int)));
    QObject::connect(op, SIGNAL(labelSelectionUpdated(QBitArray)), this, SLOT(onLabelSelectionUpdated(QBitArray)));
    QObject::connect(op, SIGNAL(labelDoubleClicked(int)), this, SLOT(onLabelDoubleClicked(int)));
}

void TranslationEditArea::onNewPage() {
    auto newPage = op->page();

    auto expectedR = newPage == nullptr ? 0 : newPage->labelCount();
    auto currentR = translationTable->rowCount();

    if (expectedR >= currentR) {
        translationTable->setRowCount(expectedR);

        for (int i = currentR; i < expectedR; ++i) {
            translationTable->setItem(i, 0, new QTableWidgetItem);
        }
    } else {
        for (int i = currentR - 1; i >= expectedR; --i) {
            delete translationTable->takeItem(i, 0);
        }

        translationTable->setRowCount(expectedR);
    }

    translationText->setEnabled(false);
    translationText->setText("");
    selectedLabel = -1;

    if (newPage == nullptr) {
        translationTable->setEnabled(false);
        return;
    }

    translationTable->setEnabled(true);

    for (int i = 0; i < expectedR; ++i) {
        translationTable->item(i, 0)->setText(newPage->label(i).translation);
    }

    translationTable->clearSelection();
}

void TranslationEditArea::configTextEdit() {
    auto selected = translationTable->selectedItems();
    auto selectedCount = selected.count();

    if (selectedCount != 1) {
        selectedLabel = -1;

        translationText->setEnabled(false);
        translationText->setText("");
    } else {
        selectedLabel = -1;

        translationText->setEnabled(true);
        translationText->setText(selected[0]->text());

        selectedLabel = selected[0]->row();
    }
}

void TranslationEditArea::deleteLabel(const QBitArray &deleted, bool notify) {
    suppressUIEvents = true;

    auto oldCount = translationTable->rowCount();
    auto deletedCount = 0;
    for (int i = oldCount - 1; i >= 0; --i) {
        if (deleted.testBit(i)) {
            delete translationTable->takeItem(i, 0);
            translationTable->removeRow(i);
            ++deletedCount;
        }
    }

    translationTable->clearSelection();

    suppressUIEvents = false;

    // actually unnecessary but ...
    translationTable->setRowCount(oldCount - deletedCount);

    if (notify) {
        suppressExternalSignal = true;
        op->deleteLabel(deleted);
        suppressExternalSignal = false;
    }
}

void TranslationEditArea::tableSelectionChanged() {
    if (suppressUIEvents) {
        return;
    }

    configTextEdit();

    auto selected = translationTable->selectedItems();
    auto selectedCount = selected.count();

    QBitArray b(op->page()->labelCount(), false);
    for (int i = 0; i < selectedCount; ++i) {
        b.setBit(selected.at(i)->row());
    }

    suppressExternalSignal = true;
    op->setLabelSelection(b);
    suppressExternalSignal = false;
}

void TranslationEditArea::tableItemDoubleClicked(QTableWidgetItem *item) {
    op->doubleClickLabel(translationTable->currentRow());
}

void TranslationEditArea::textEdited() {
    if (selectedLabel < 0 || suppressUIEvents) {
        return;
    }

    auto text = translationText->toPlainText();

    translationTable->item(selectedLabel, 0)->setText(text);

    suppressExternalSignal = true;
    op->setLabelContent(selectedLabel, text);
    suppressExternalSignal = false;
}

void TranslationEditArea::deleteKeyPressed() {
    auto selected = translationTable->selectedItems();
    auto selectedCount = selected.count();

    QBitArray b(op->page()->labelCount(), false);
    for (int i = 0; i < selectedCount; ++i) {
        b.setBit(selected.at(i)->row());
    }

    deleteLabel(b);
}

void TranslationEditArea::onLabelAppended() {
    if (suppressExternalSignal) {
        return;
    }

    auto newRowCount = op->page()->labelCount();
    translationTable->setRowCount(newRowCount);
    translationTable->setItem(newRowCount - 1, 0, new QTableWidgetItem(op->page()->label(newRowCount - 1).translation));
}

void TranslationEditArea::onLabelDeleted(QBitArray deleted) {
    if (suppressExternalSignal) {
        return;
    }

    deleteLabel(deleted, false);
}

void TranslationEditArea::onLabelContentUpdated(int index) {
    if (suppressExternalSignal) {
        return;
    }

    translationTable->item(index, 0)->setText(op->page()->label(index).translation);
}

void TranslationEditArea::onLabelSelectionUpdated(QBitArray selected) {
    if (suppressExternalSignal) {
        return;
    }

    suppressUIEvents = true;

    translationTable->setSelectionMode(QAbstractItemView::MultiSelection);
    translationTable->clearSelection();

    int len = op->page()->labelCount();
    int cnt = 0;
    for (int i = 0; i < len; ++i) {
        if (selected.testBit(i)) {
            translationTable->selectRow(i);
            ++cnt;
        }
    }

    translationTable->setSelectionMode(QAbstractItemView::ExtendedSelection);

    configTextEdit();

    suppressUIEvents = false;
}

void TranslationEditArea::onLabelDoubleClicked(int index) {
    translationText->setFocus();
}
