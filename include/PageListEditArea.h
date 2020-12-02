#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolButton>
#include <QListWidget>

class PageListEditArea : public QWidget {
    Q_OBJECT

private:
    QVBoxLayout *pageListLayout;
    QHBoxLayout *pageListEditorLayout;
    QToolButton *pageListEdit, *pageListEditDone, *pageListAdd, *pageListRemove, *pageListToTop, *pageListToBottom;
    QListWidget *pageList;

public:
    explicit PageListEditArea(QWidget *parent = nullptr);
    ~PageListEditArea() override = default;
};
