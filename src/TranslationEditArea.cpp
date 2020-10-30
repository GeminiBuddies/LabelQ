#include <TranslationEditArea.h>

#include <model/Page.h>
#include <model/PageOperator.h>

#include <QKeyEvent>
#include <QHeaderView>

QTableWidgetWithKeySignal::QTableWidgetWithKeySignal(QWidget *parent) : QTableWidget(parent) {

}

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

    translationText = new QTextEdit(splitter);

    splitter->addWidget(translationTable);
    splitter->addWidget(translationText);

    verticalLayout->addWidget(splitter);

    QObject::connect(translationTable, SIGNAL(itemSelectionChanged()), this, SLOT(tableSelectionChanged()));

    op = nullptr;

    processingExternalSignal = false;
    suppressExternalSignal = false;
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

    translationText->setText("");

    if (newPage == nullptr) {
        translationTable->setEnabled(false);
        translationText->setEnabled(false);
        return;
    }

    translationTable->setEnabled(true);
    translationText->setEnabled(true);

    for (int i = 0; i < expectedR; ++i) {
        translationTable->item(i, 0)->setText(newPage->label(i).translation);
    }
}

void TranslationEditArea::tableSelectionChanged() {
    if (processingExternalSignal) return;

    auto selected = translationTable->selectedItems();
    auto selectedCount = selected.count();

    if (selectedCount != 1) {
        translationText->setEnabled(false);
        translationText->setText("");
    }

    QBitArray b(op->page()->labelCount(), false);
    for (int i = 0; i < selectedCount; ++i) {
        b.setBit(selected.at(i)->row());
    }

    suppressExternalSignal = true;
    op->setSelection(b);
    suppressExternalSignal = false;
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

    auto oldCount = translationTable->rowCount();
    for (int i = oldCount - 1; i >= 0; --i) {
        if (deleted.testBit(i)) {
            delete translationTable->takeItem(i, 0);
            translationTable->removeRow(i);
        }
    }

    // actually unnecessary but ...
    translationTable->setRowCount(op->page()->labelCount());
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

    processingExternalSignal = true;

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

    if (cnt != 1) {
        translationText->setText("");
        translationText->setEnabled(false);
    }

    processingExternalSignal = false;
}
