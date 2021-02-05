#pragma once

#include <QDialog>
#include <QLayout>

class QuickInputLayout : public QLayout {
    Q_OBJECT

private:
    QVector<QLayoutItem*> items;
    int w;

public:
    QuickInputLayout(QWidget *parent, int w);

    void addItem(QLayoutItem *item) override;
    int count() const override;
    QLayoutItem* itemAt(int index) const override;
    QLayoutItem* takeAt(int index) override;

    QSize minimumSize() const override;
    QSize sizeHint() const override;
    void setGeometry(const QRect &rect) override;

private:
    QSize doLayout(const QRect &rect, bool calcOnly) const;
};

class QuickInput : public QDialog {
    Q_OBJECT

private:
    const int w = 300;
    QStringList words;
    int wordCount;

public:
    explicit QuickInput(QWidget *parent = nullptr, const QStringList &words = QStringList());
    ~QuickInput() override = default;

protected:
    void keyPressEvent(QKeyEvent *myEvent) override;

private:
    int selected = -1;
    QString emptyStr = QString();

public:
    const QString& popup(const QPoint &pos);
};
