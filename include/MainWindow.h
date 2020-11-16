#pragma once

#include <QMainWindow>
#include <model/Model.h>
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

    void showAboutMessage();
    void showAboutQtMessage();

// Project and Page management
private:
    Project *project;
    int currentPageIndex;
    Page *currentPage;
    PageOperator *op;

    bool pageEditEnabled;

    bool replaceProject(Project *newProject);
    void setCurrentPage(int index);

    QVector<int> getSelectedPages();
    QVector<int> getSortedSelectedPages();

private slots:
    void togglePageEditing();
    void disablePageEditing();

    void toTop();
    void toBottom();

    void pageListReordered(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row);
    void pageListSelectionItemChanged();

public slots:
    void showTutorial();
};
