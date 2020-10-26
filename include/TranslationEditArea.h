#pragma once

#include <model/Model.h>

#include <QWidget>
#include <QVBoxLayout>
#include <QSplitter>
#include <QTableWidget>
#include <QTextEdit>

class TranslationEditArea : public QWidget {
    Q_OBJECT

private:
    QVBoxLayout *verticalLayout;
    QSplitter *splitter;
    QTableWidget *translationTable;
    QTextEdit *translationText;

    Page *_page;

public:
    explicit TranslationEditArea(QWidget *parent = nullptr);
    ~TranslationEditArea() override;

    void setPage(Page *page);

public slots:
    void reloadPage();
};
