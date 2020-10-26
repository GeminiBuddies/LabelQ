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

    Page *_page;

public:
    explicit TranslationEditArea(QWidget *parent = nullptr);
    ~TranslationEditArea() override;

    void setPage(Page *page);

private slots:
    void tableSelectionChanged();

public slots:
    void reloadPage();
};
