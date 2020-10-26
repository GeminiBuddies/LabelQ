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
}

TranslationEditArea::~TranslationEditArea() {}

void TranslationEditArea::setPage(Page *page) {
    _page = page;
    reloadPage();
}

void TranslationEditArea::tableSelectionChanged() {
    auto selected = translationTable->selectedItems();

    if (selected.count() != 1) {
        translationText->setText("");
        translationText->setEnabled(false);
    } else {
        selected[0]->row();
        translationText
    }
}

void TranslationEditArea::reloadPage() {
    // very slow, very stupid, but it works
    auto r = translationTable->rowCount();

    for (int i = 0; i < r; ++i) {
        delete translationTable->takeItem(i, 0);
    }

    translationTable->setRowCount(0);

    translationText->setText("");

    if (_page == nullptr) {
        translationTable->setEnabled(false);
        translationText->setEnabled(false);
        return;
    }

    translationTable->setEnabled(true);
    translationText->setEnabled(true);

    auto len = _page->labelCount();
    translationTable->setRowCount(len);
    for (int i = 0; i < len; ++i) {
        translationTable->setItem(i, 0, new QTableWidgetItem(_page->label(i).translation));
    }
}
