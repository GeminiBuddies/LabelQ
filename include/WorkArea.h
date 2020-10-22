#pragma once

#include <QLabel>

class WorkArea : public QLabel {
    Q_OBJECT

public:
    explicit WorkArea(QWidget *parent = nullptr);
    ~WorkArea() override = default;

public slots:
    void zoom(bool in);
};
