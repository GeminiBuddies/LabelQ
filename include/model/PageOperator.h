#pragma once

#include "Model.h"

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

public:
    PageOperator();

    void setPage(Page *page);
    Page* page();
    int appendLabel(const QPoint &position);
    void deleteLabel(const QBitArray& deleted);
    void setLabelContent(int index, const QString &content);
    void setSelection(const QBitArray &selected);
};
