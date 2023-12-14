#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidgetAction>
#include <QLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QMap>
#include <QDebug>
//#include <QSqlDatabase>
//#include <QSqlQuery>
//#include <QSqlTableModel>

enum Type
{
    COPY,
    MIRROR
};

enum Days
{
    MON = 1,
    TUE = 2,
    WED = 4,
    THU = 8,
    FRI = 16,
    SAT = 32,
    SUN = 64,
    ALL = 128
};

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE
class ActionLineEdit;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void pushButtonDelete_clicked();

    void on_actionSourcePath_triggered();

    void on_actionCopyPath_triggered();

    void on_actionAddTask_triggered();

    void on_actionDelTask_triggered();

    void on_pushButtonReadBase_clicked();

    void on_tableWidget_cellClicked(int row, int column);

    void on_actionDays_triggered(bool checked);

    void on_actionType_triggered(bool checked);

    void on_pushButtonOk_clicked();

    void on_pushButtonDayAll_clicked();

    void on_pushButtonDay1_clicked();

    void on_pushButtonDay2_clicked();

    void on_pushButtonDay3_clicked();

    void on_pushButtonDay4_clicked();

    void on_pushButtonDay5_clicked();

    void on_pushButtonDay6_clicked();

    void on_pushButtonDay7_clicked();

    void on_pushButtonDayReset_clicked();

    void on_pushButtonTypeCopy_clicked(bool checked);

    void on_pushButtonTypeMirror_clicked(bool checked);

    void on_actionNewTask_triggered();

private:
    Ui::MainWindow *ui;

    QWidgetAction *actionName = nullptr;
    QLineEdit *lineEditTaskName = nullptr;
    QSpinBox *lineEditCopyNum = nullptr;
    QWidgetAction *actionLabelName = nullptr;

    void hideWidgets();

    void setActions();
    void deleteTask();
    void showTask(int row);

    void readReg();
    void writeReg();
    void showTasksTable();

    void clearTaskInfo();

    int getSelectedType();
    int getSelectedDays();
    void resetDays();
    void setDays(int days);
    void checkDaysButton(QPushButton *button);

    void createActionCopyNum();

    QString getPath(const QString winName);
};
#endif // MAINWINDOW_H
