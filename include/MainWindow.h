#pragma once

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    Ui::MainWindow *ui;

protected:
    void showEvent(QShowEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    QPixmap pic;
    int percentage;
    void zoom(bool in);

private slots:
    void zoomIn();
    void zoomOut();

    void adjustWorkAreaMargin();
    void customUiSetup();

    void x(int r, int c);
};
