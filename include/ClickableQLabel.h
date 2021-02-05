#pragma once

#include <QLabel>

class ClickableQLabel : public QLabel {
    Q_OBJECT

public:
    ClickableQLabel(const QString &, QWidget * = nullptr);

signals:
    void clicked();

protected:
    bool event(QEvent *myEvent);
};
