#pragma once

#include <QMainWindow>
#include <model/Model.h>
#include <lib/DialogProvider.h>
#include <controller/Controller.h>

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

private slots:
    void adjustWorkAreaMargin();
    void customUiSetup();

    void selectPrevPage();
    void selectNextPage();

private slots:
    // menu handlers
    void newProject();
    void openProject();
    void saveProject();
    void saveProjectAs();
    void exportLabelPlus();
    void closeProject();

    void showTutorial();

    void exitProgram();

    void showAboutMessage();
    void showAboutQtMessage();

// Project and Page management
private:
    DialogProvider *dp;
    ProjectOperator *prop;
    PageOperator *op;
};
