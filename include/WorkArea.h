#pragma once

#include <model/Page.h>

#include <QLabel>

class WorkArea : public QLabel {
    Q_OBJECT

private:
    Page *_currentPage;

public:
    explicit WorkArea(QWidget *parent = nullptr);
    ~WorkArea() override = default;

    Page* page() const;
    void setPage(Page *page);

public slots:
    void zoom(bool in);
};
