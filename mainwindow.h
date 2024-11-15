#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

constexpr int AGRADE_MIN = 90;
constexpr int AGRADE_MAX = 100;
constexpr int BGRADE_MIN = 70;
constexpr int BGRADE_MAX = 89;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    void navigateToHomePage();
    void connectToDB();
    void disconnectDB();
    std::string getGrade(int);
    ~MainWindow();

private slots:
    void on_addST_clicked();

    void on_ADD_clicked();

    void on_addGRD_clicked();

    void on_add2_clicked();

    void on_search_clicked();

    void on_searchST_clicked();

    void on_listST_clicked();

    void on_STreport_clicked();

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
