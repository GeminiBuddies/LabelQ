#pragma once

#include <controller/ProjectOperator.h>

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolButton>
#include <QListWidget>

#include <controller/ProjectOperator.h>

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

// Project and Page management
private:
    ProjectOperator *op;
    bool pageEditEnabled;
    bool suppressExternalSignal;

    QVector<int> getSelectedPages();
    QVector<int> getSortedSelectedPages();

private slots:
    void projectReplaced();

    void togglePageEditing();
    void disablePageEditing();

    void addPage();
    void removePage();
    void toTop();
    void toBottom();

    void pageListSelectionItemChanged();
    void pageSelectionChanged();

public:
    void setProjectOperator(ProjectOperator *pOperator);
};
