#ifndef ENTRYWINDOW_H
#define ENTRYWINDOW_H

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

private slots:
    void changeLanguage();
    void appendText();
    void customUiSetup();
};
#endif // ENTRYWINDOW_H
