#pragma once

#include <model/Model.h>

#include <QWidget>
#include <QVBoxLayout>
#include <QSplitter>
#include <QTableWidget>
#include <QTextEdit>

#include <QBitArray>

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

    PageOperator *op;

public:
    explicit TranslationEditArea(QWidget *parent = nullptr);
    ~TranslationEditArea() override;

    void setPageOperator(PageOperator *op);

private:
    bool processingExternalSignal;
    bool suppressExternalSignal;

    int selectedLabel;

private slots:
    void tableSelectionChanged();
    void textEdited();

public slots:
    void onNewPage();

    void onLabelAppended();
    void onLabelDeleted(QBitArray deleted);
    void onLabelContentUpdated(int index);
    void onLabelSelectionUpdated(QBitArray selected);
};