#pragma once

#include <QDialog>

class QuickInput : public QDialog {
    Q_OBJECT

public:
    explicit QuickInput(QWidget *parent = nullptr);
    ~QuickInput() override = default;
};
