#pragma once

#include <QMainWindow>
#include <model/Model.h>

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

private slots:
    void zoom(bool in);

    void adjustWorkAreaMargin();
    void customUiSetup();

    void x(int r, int c);

// Project and Page management
private:
    Project *project;
    int currentPageIndex;
    Page *currentPage;
    PageOperator *op;

    bool replaceProject(Project *newProject);
    void setCurrentPage(int index);

signals:
    void currentPageChanged(Page *newPage);
};
