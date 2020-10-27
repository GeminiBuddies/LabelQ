#pragma once

#include <model/Model.h>

#include <QWidget>
#include <QVBoxLayout>
#include <QSplitter>
#include <QTableWidget>
#include <QTextEdit>

class QTableWidgetWithKeySignal : public QTableWidget {
    Q_OBJECT

public:
    explicit QTableWidgetWithKeySignal(QWidget *parent);
    ~QTableWidgetWithKeySignal() override = default;

protected:
    void keyPressEvent(QKeyEvent *event) override;

signals:
    void deleteKeyPressed();
};

class TranslationEditArea : public QWidget {
    Q_OBJECT

private:
    QVBoxLayout *verticalLayout;
    QSplitter *splitter;
    QTableWidgetWithKeySignal *translationTable;
    QTextEdit *translationText;

    Page *page;

public:
    explicit TranslationEditArea(QWidget *parent = nullptr);
    ~TranslationEditArea() override;

private:
    bool processingExternalSignal;

private slots:
    void tableSelectionChanged();

signals:
    void labelDeleted(QBitArray *deleted);
    void labelContentUpdated(int index);
    void labelSelectionUpdated(QBitArray *selected);

public slots:
    void setPage(Page *newPage);
    void onLabelAppended(QWidget *sender);
    void onLabelDeleted(QWidget *sender, QBitArray *deleted);
    void onLabelContentUpdated(QWidget *sender, int index);
    void onLabelSelectionUpdated(QWidget *sender, QBitArray *selected);
};
