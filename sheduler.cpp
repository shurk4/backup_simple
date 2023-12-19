#include "sheduler.h"

Sheduler::Sheduler(QObject *parent)
    : QObject{parent}
{
    updateSettings();
    debugSettings();
    checkTasks();
    startTimer();
}

bool Sheduler::isReady()
{
    return ready;
}

void Sheduler::debugSettings()
{
    qDebug() << "\nSettings";
    for(const auto &i : tasks)
    {
        qDebug() << "name: " << i.name << " ";
        qDebug() << "sourcePath: " << i.sourcePath << " ";
        qDebug() << "copyPath: " << i.copyPath << " ";
        qDebug() << "bitPath: " << i.backInTimePath;
        qDebug() << "type: " << i.type << " ";
        qDebug() << "days: " << i.days << " ";
        qDebug() << "copyNum: " << i.copyNum << " ";
        qDebug() << "time: " << i.time.toString("hh:mm") << " ";
        qDebug() << "lastRun: " << i.lastRun.toString() << " ";
        qDebug() << "lastComplite: " << i.lastComplite.toString() << " ";
        qDebug() << "proceed: " << i.proceed << " ";
        qDebug();
    }
}

void Sheduler::update()
{
    qDebug() << "Update sheduler in thread: " << QThread::currentThreadId();
    tasks.clear();
    updateSettings();
    checkTasks();
    debugSettings();
}

void Sheduler::saveRegSlot()
{
    QSettings settings("ShurkSoft", "Simple BackUp");
    settings.beginGroup("Tasks");
    for(const auto i : tasks)
    {
        settings.beginGroup(i.name);
        settings.setValue("time", i.time);
        settings.setValue("lastRun", i.lastRun);
        settings.setValue("lastComplite", i.lastComplite);
        settings.setValue("proceed", i.proceed);
        settings.endGroup();
    }
    settings.endGroup();
}

void Sheduler::timerAlarm()
{
    qDebug() << "\nCerrent time: " << QTime::currentTime().toString("hh:mm");
    checkTasks();
}

void Sheduler::startTimer()
{    
    qDebug() << "\nSheduler started in thread: " << QThread::currentThreadId();
    qDebug();
    timer = new QTimer();
    connect(timer, &QTimer::timeout, this, &Sheduler::timerAlarm);
//    timer->start(240000); // 4 минуты
    timer->start(30000);
}

void Sheduler::updateSettings()
{
    QSettings settings("ShurkSoft", "Simple BackUp");

    settings.beginGroup("Tasks");
    QStringList keys = settings.childGroups();
    for(const auto i : keys)
    {
        settings.beginGroup(i);
        tasks[i].name = i;
        tasks[i].sourcePath = settings.value("sourcePath").toString();
        tasks[i].copyPath = settings.value("copyPath").toString();
        tasks[i].backInTimePath = settings.value("bitPath").toString();
        tasks[i].days = settings.value("days").toInt();
        tasks[i].type = settings.value("type").toInt();
        tasks[i].copyNum = settings.value("copysNum").toInt();
        tasks[i].time = settings.value("time").toTime();
        tasks[i].lastRun = settings.value("lastRun").toDateTime();
        tasks[i].lastComplite = settings.value("lastComplite").toDateTime();
        tasks[i].proceed = settings.value("proceed").toBool();


        settings.endGroup();
    }
    settings.endGroup();
    ready = true;
}

void Sheduler::checkTasks()
{
    qDebug() << "Check tasks";
    for(const auto &i : tasks)
    {
        if(i.time.hour() == QTime::currentTime().hour() && i.time.minute() <= (QTime::currentTime().minute() + 5) && !i.proceed && (i.lastComplite.date().day() - 1) < QDate::currentDate().day()) // Отключена проверка на последний запуск, убрать - 1
        {
            qDebug() << "Task " << i.name << "will be started";
            runTask(i.name);
        }
        else
        {
            qDebug() << "Task " << i.name  << "Not available task";
        }
    }
    qDebug();
}

void Sheduler::runTask(QString name)
{
    Task *task = new Task(&tasks[name], &mutex);
    connect(task, &Task::saveRegSignal, this, &Sheduler::saveRegSlot);
    task->setAutoDelete(true);
    QThreadPool::globalInstance()->start(task);
}
