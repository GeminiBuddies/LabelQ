#include <TranslationEditArea.h>

#include <model/Page.h>

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
    translationTable->setHorizontalHeaderItem(0, new QTableWidgetItem(tr("entryWindow_textTable_textHeader")));
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

    setPage(nullptr);
    processingExternalSignal = false;
}

TranslationEditArea::~TranslationEditArea() {}

void TranslationEditArea::setPage(Page *newPage) {
    page = newPage;

    auto expectedR = page == nullptr ? 0 : page->labelCount();
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
        translationText->setText("");
        translationText->setEnabled(false);
    }

    QBitArray b(page->labelCount(), false);
    for (int i = 0; i < selectedCount; ++i) {
        b.setBit(selected.at(i)->row());
    }

    emit labelSelectionUpdated(&b);
}

void TranslationEditArea::onLabelAppended(QWidget *sender) {
    if (sender == this) {
        return;
    }
}

void TranslationEditArea::onLabelDeleted(QWidget *sender, QBitArray *deleted) {
    if (sender == this) {
        return;
    }
}

void TranslationEditArea::onLabelContentUpdated(QWidget *sender, int index) {
    if (sender == this) {
        return;
    }
}

void TranslationEditArea::onLabelSelectionUpdated(QWidget *sender, QBitArray *selected) {
    if (sender == this) {
        return;
    }

    processingExternalSignal = true;
    translationTable->setSelectionMode(QAbstractItemView::MultiSelection);

    translationTable->clearSelection();

    int len = page->labelCount();
    int cnt = 0;
    for (int i = 0; i < len; ++i) {
        if (selected->testBit(i)) {
            translationTable->selectRow(i);
            ++cnt;
        }
    }

    translationTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
    processingExternalSignal = false;

    if (cnt != 1) {
        translationText->setText("");
        translationText->setEnabled(false);
    }
}
