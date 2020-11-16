#pragma once

#include "./Controller.h"

#include <model/Model.h>

#include <QObject>
#include <QBitArray>

Q_DECLARE_METATYPE(QBitArray)

class PageOperator : public QObject {
    Q_OBJECT

private:
    Page *currentPage;

signals:
    void newPageSet();
    void labelAppended();
    void labelDeleted(QBitArray deleted);
    void labelContentUpdated(int index);
    void labelSelectionUpdated(QBitArray selected);
    void labelDoubleClicked(int index);

public:
    PageOperator();

    void setPage(Page *page);
    Page* page();
    int appendLabel(const QPoint &position);
    void deleteLabel(const QBitArray &deleted);
    void setLabelContent(int index, const QString &content);
    void setLabelSelection(const QBitArray &selected);
    void doubleClickLabel(int index);
};
