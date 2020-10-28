#pragma once

#include <QScrollArea>
#include <QWheelEvent>

class ZoomScrollArea : public QScrollArea {
    Q_OBJECT

public:
    explicit ZoomScrollArea(QWidget *parent = nullptr);
    ~ZoomScrollArea() override;

protected:
    void wheelEvent(QWheelEvent *event) override;

signals:
    void zoom(bool in);
};
