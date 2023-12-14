#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setActions();
    readReg();

    hideWidgets();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::pushButtonDelete_clicked()
{
    deleteTask();
}

void MainWindow::setActions()
{
    actionLabelName = new QWidgetAction(this);

    QLabel *label = new QLabel("Задание: ", this);
    actionLabelName->setDefaultWidget(label);
    ui->toolBar->insertAction(ui->actionSourcePath, actionLabelName);

    actionName = new QWidgetAction(this);

    lineEditTaskName = new QLineEdit(this);
    lineEditTaskName->setMaximumWidth(200);
    lineEditTaskName->setPlaceholderText("Введите имя задания");
    actionName->setDefaultWidget(lineEditTaskName);
    ui->toolBar->insertAction(ui->actionSourcePath, actionName);

    createActionCopyNum();
}

void MainWindow::deleteTask()
{
    int row = ui->tableWidget->currentRow();
    if(row >= 0)
    {
        QString taskName = ui->tableWidget->item(row, 1)->text();

        QSettings settings("ShurkSoft", "Simple BackUp");
        settings.beginGroup("Tasks");
        settings.remove(taskName);
        settings.endGroup();
    }
    else
    {
        QMessageBox::information(this, "", "Не удалось удалить");
    }
    showTasksTable();
}

void MainWindow::showTask(int row)
{
    QString taskName = ui->tableWidget->item(row, 1)->text();

    QSettings settings("ShurkSoft", "Simple BackUp");
    settings.beginGroup("Tasks");
    settings.beginGroup(taskName);
    ui->labelName->setText(taskName);
    ui->labelSourcePath->setText(settings.value("sourcePath").toString());
    ui->labelCopyPath->setText(settings.value("copyPath").toByteArray());
    lineEditTaskName->setText(taskName);
    setDays(settings.value("days").toInt());
    ui->labelDays->setText(getDaysString(settings.value("days").toInt()));
    setType(settings.value("type").toInt());
    lineEditCopyNum->setValue(settings.value("copysNum").toInt());

    settings.endGroup();
    settings.endGroup();

    ui->actionAddTask->setText("Сохранить");
    taskShowed = false;
}

void MainWindow::readReg()
{
    clearTaskInfo();
    showTasksTable();
}

void MainWindow::writeReg()
{
    QSettings settings("ShurkSoft", "Simple BackUp");
    settings.beginGroup("Tasks");
        settings.beginGroup(lineEditTaskName->text());
            settings.setValue("sourcePath", ui->labelSourcePath->text());
            settings.setValue("copyPath", ui->labelCopyPath->text());
            settings.setValue("days", getSelectedDays());
            settings.setValue("type", getSelectedType());
            settings.setValue("copysNum", lineEditCopyNum->text().toInt());
        settings.endGroup();
    settings.endGroup();

    clearTaskInfo();
    showTasksTable();
}

void MainWindow::showTasksTable()
{
    QSettings settings("ShurkSoft", "Simple BackUp");

    settings.beginGroup("Tasks");
    QStringList keys = settings.childGroups();

    ui->tableWidget->setRowCount(keys.size());
    ui->tableWidget->setColumnCount(6);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() << " " << "Имя" << "Пусть источник" << "Путь копии" << "Дни" << "Тип");
    ui->tableWidget->setColumnWidth(0, 5);
    ui->tableWidget->setColumnWidth(1, 70);
    ui->tableWidget->setColumnWidth(2, 120);
    ui->tableWidget->setColumnWidth(3, 120);
    ui->tableWidget->setColumnWidth(4, 120);
    for(int i = 0 ; i < keys.size(); i++)
    {
        settings.beginGroup(keys[i]);

        QTableWidgetItem *item;

        item = new QTableWidgetItem();
        QPushButton *btn =  new QPushButton("-");
        btn->setMaximumSize(34, 34);
        item->setTextAlignment(Qt::AlignCenter);
        connect(btn, &QPushButton::clicked, this, &MainWindow::pushButtonDelete_clicked);
        ui->tableWidget->setCellWidget(i, 0, btn);

        item = new QTableWidgetItem();
        item->setText(keys[i]);
        ui->tableWidget->setItem(i, 1, item);

        item = new QTableWidgetItem();
        item->setText(settings.value("sourcePath").toString());
        item->setToolTip(settings.value("sourcePath").toString());
        ui->tableWidget->setItem(i, 2, item);

        item = new QTableWidgetItem();
        item->setText(settings.value("copyPath").toString());
        item->setToolTip(settings.value("copyPath").toString());
        ui->tableWidget->setItem(i, 3, item);

        item = new QTableWidgetItem();
        item->setText(getDaysString(settings.value("days").toInt()));
        ui->tableWidget->setItem(i, 4, item);

        item = new QTableWidgetItem();
        item->setText(getTypeString(settings.value("type").toInt()));
        ui->tableWidget->setItem(i, 5, item);
        settings.endGroup();
    }

    taskShowed = true;
}

void MainWindow::clearTaskInfo()
{
    ui->labelName->clear();
    ui->labelSourcePath->clear();
    ui->labelCopyPath->clear();
    ui->labelDays->clear();
    lineEditTaskName->clear();
}

int MainWindow::getSelectedType()
{
    int type;
    if(ui->pushButtonTypeCopy->isChecked())
    {
        type = COPY;
    }
    else
    {
        type = MIRROR;
    }
    return type;
}

void MainWindow::setType(int type)
{
    if(type == COPY)
    {
        ui->pushButtonTypeCopy->setChecked(true);
    }
    else
    {
        ui->pushButtonTypeMirror->setChecked(true);
    }
}

QString MainWindow::getTypeString(int type)
{
    if(type == COPY)
    {
        return "Копия";
    }
    else
    {
        return "Зеркало";
    }
}

int MainWindow::getSelectedDays()
{
    int day = 0;

    if(ui->pushButtonDayAll->isChecked())
    {
        day |= ALL;
    }

    if(ui->pushButtonDay1->isChecked())
    {
        day |= MON;
    }

    if(ui->pushButtonDay2->isChecked())
    {
        day |= TUE;
    }

    if(ui->pushButtonDay3->isChecked())
    {
        day |= WED;
    }

    if(ui->pushButtonDay4->isChecked())
    {
        day |= THU;
    }

    if(ui->pushButtonDay5->isChecked())
    {
        day |= FRI;
    }

    if(ui->pushButtonDay6->isChecked())
    {
        day |= SAT;
    }

    if(ui->pushButtonDay7->isChecked())
    {
        day |= SUN;
    }

    return day;
}

QString MainWindow::getDaysString(int days)
{
    QString day = "";

    if(days & ALL)
    {
        day += "Каждый день";
        return day;
    }

    if(days & MON)
    {
        day += "Пн ";
    }

    if(days & TUE)
    {
        day += "Вт ";
    }

    if(days & WED)
    {
        day += "Ср ";
    }

    if(days & THU)
    {
        day += "Чт ";
    }

    if(days & FRI)
    {
        day += "Пт ";
    }

    if(days & SAT)
    {
        day += "Сб ";
    }

    if(days & SUN)
    {
        day += "Вс ";
    }

    ui->labelDays->setText(day);
}

void MainWindow::resetDays()
{
    ui->pushButtonDay1->setChecked(false);
    ui->pushButtonDay2->setChecked(false);
    ui->pushButtonDay3->setChecked(false);
    ui->pushButtonDay4->setChecked(false);
    ui->pushButtonDay5->setChecked(false);
    ui->pushButtonDay6->setChecked(false);
    ui->pushButtonDay7->setChecked(false);
}

void MainWindow::setDays(int days)
{
    QString day = "";
    resetDays();

    if(days & ALL)
    {
        ui->pushButtonDay1->setChecked(true);
        return;
    }

    if(days & MON)
    {
        ui->pushButtonDay1->setChecked(true);
    }

    if(days & TUE)
    {
        ui->pushButtonDay2->setChecked(true);
    }

    if(days & WED)
    {
        ui->pushButtonDay3->setChecked(true);
    }

    if(days & THU)
    {
        ui->pushButtonDay4->setChecked(true);
    }

    if(days & FRI)
    {
        ui->pushButtonDay5->setChecked(true);
    }

    if(days & SAT)
    {
        ui->pushButtonDay6->setChecked(true);
    }

    if(days & SUN)
    {
        ui->pushButtonDay7->setChecked(true);
    }
}

void MainWindow::checkDaysButton(QPushButton *button)
{
    if(ui->pushButtonDayAll->isChecked())
    {
        ui->pushButtonDayAll->setChecked(false);
        resetDays();
        button->setChecked(true);
    }
}

void MainWindow::createActionCopyNum()
{
    QWidgetAction *actionCopyNumName = nullptr;
    actionCopyNumName = new QWidgetAction(this);
    QLabel *label = new QLabel("Количество копий: ", this);
    actionCopyNumName->setDefaultWidget(label);
    ui->toolBar->insertAction(ui->actionAddTask ,actionCopyNumName);

    ui->toolBar->insertSeparator(actionCopyNumName);

    QWidgetAction *actionCopyNum = new QWidgetAction(this);
    lineEditCopyNum = new QSpinBox(this);
    lineEditCopyNum->setMaximumWidth(100);
    lineEditCopyNum->setMaximum(10);
    lineEditCopyNum->setMinimum(0);
    actionCopyNum->setDefaultWidget(lineEditCopyNum);
    ui->toolBar->insertAction(ui->actionAddTask ,actionCopyNum);

    ui->toolBar->insertSeparator(ui->actionAddTask);
}

QString MainWindow::getPath(const QString winName)
{
    return QFileDialog::getExistingDirectory(this, winName);
}

void MainWindow::on_actionSourcePath_triggered()
{
    ui->labelSourcePath->setText(getPath("Выбирите папку с исходными данными."));
}

void MainWindow::on_actionCopyPath_triggered()
{
    ui->labelCopyPath->setText(getPath("ВЫбирите папку для сохранения копий."));
}

void MainWindow::on_actionAddTask_triggered()
{
    writeReg();
}

void MainWindow::on_actionDelTask_triggered()
{
    deleteTask();
}

void MainWindow::on_pushButtonReadBase_clicked()
{
    readReg();
}

void MainWindow::on_tableWidget_cellClicked(int row, int column)
{
    showTask(row);
}

void MainWindow::hideWidgets()
{
    ui->widgetDays->hide();
    ui->widgetType->hide();
    ui->pushButtonOk->hide();
    ui->actionDays->setChecked(false);
    ui->actionType->setChecked(false);
}

void MainWindow::on_actionDays_triggered(bool checked)
{
    if(checked)
    {
        ui->widgetDays->show();
        ui->pushButtonOk->show();
    }
    else
    {
        ui->widgetDays->hide();
        if(!ui->actionType->isChecked())
        {
            ui->pushButtonOk->hide();
        }
    }
}

void MainWindow::on_actionType_triggered(bool checked)
{
    if(checked)
    {
        ui->widgetType->show();
        ui->pushButtonOk->show();
    }
    else
    {
        ui->widgetType->hide();
        if(!ui->actionDays->isChecked())
        {
            ui->pushButtonOk->hide();
        }
    }
}

void MainWindow::on_pushButtonOk_clicked()
{
    hideWidgets();
}

void MainWindow::on_pushButtonDayAll_clicked()
{
    if(ui->pushButtonDayAll->isChecked())
    {
        ui->pushButtonDay1->setChecked(true);
        ui->pushButtonDay2->setChecked(true);
        ui->pushButtonDay3->setChecked(true);
        ui->pushButtonDay4->setChecked(true);
        ui->pushButtonDay5->setChecked(true);
        ui->pushButtonDay6->setChecked(true);
        ui->pushButtonDay7->setChecked(true);
    }
    else
    {
        resetDays();
    }
}

void MainWindow::on_pushButtonDay1_clicked()
{
    checkDaysButton(ui->pushButtonDay1);
}

void MainWindow::on_pushButtonDay2_clicked()
{
    checkDaysButton(ui->pushButtonDay2);
}

void MainWindow::on_pushButtonDay3_clicked()
{
    checkDaysButton(ui->pushButtonDay3);
}

void MainWindow::on_pushButtonDay4_clicked()
{
    checkDaysButton(ui->pushButtonDay4);
}

void MainWindow::on_pushButtonDay5_clicked()
{
    checkDaysButton(ui->pushButtonDay5);
}

void MainWindow::on_pushButtonDay6_clicked()
{
    checkDaysButton(ui->pushButtonDay6);
}

void MainWindow::on_pushButtonDay7_clicked()
{
    checkDaysButton(ui->pushButtonDay7);
}

void MainWindow::on_pushButtonDayReset_clicked()
{
    resetDays();
    ui->pushButtonDayAll->setChecked(false);
}

void MainWindow::on_pushButtonTypeCopy_clicked(bool checked)
{
    if(checked)
    {
        lineEditCopyNum->setEnabled(true);
    }
}

void MainWindow::on_pushButtonTypeMirror_clicked(bool checked)
{
    if(checked)
    {
        lineEditCopyNum->setDisabled(true);
    }
}

void MainWindow::on_actionNewTask_triggered()
{
    clearTaskInfo();
    ui->actionAddTask->setText("Добавить");
}


void MainWindow::on_pushButtonStartSheduler_clicked()
{
    if(sheduler != nullptr)
    {
        sheduler->deleteLater();
    }

    sheduler = new Sheduler();
    shedulerThread = new QThread();

    sheduler->moveToThread(shedulerThread);
    shedulerThread->start();
}

