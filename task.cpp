#include "task.h"

Task::Task(TaskInfo *_taskInfo, QMutex *_mtx, QObject *parent)
    : QObject{parent}, taskInfo(_taskInfo), mutex(_mtx)
{

}

void Task::timerAlarm()
{
    mutex->lock();
    qDebug() << "Thread: " << QThread::currentThreadId() << " Task: " << taskInfo->name << " Time: " << QTime::currentTime();
    mutex->unlock();
}

void Task::startTimer()
{
    timer = new QTimer();
    connect(timer, &QTimer::timeout, this, &Task::timerAlarm);
    timer->start(800);
}

void Task::waitTime()
{
    int sleepTime = 60;
    while(taskInfo->time.minute() > QTime::currentTime().minute())
    {
        if(taskInfo->time.minute() - QTime::currentTime().minute() == 1)
        {
            sleepTime = 1;
        }

        qDebug() << "Task " << taskInfo->name << " wait time " << "QTime::currentTime().minute() = " << QTime::currentTime().minute();
        QThread::sleep(sleepTime);
    }
}

void Task::debugTaskInfo()
{
    mutex->lock();
    qDebug("--- Task info");
    qDebug() << "name: " << taskInfo->name << " ";
    qDebug() <<  "sourcePath: " << taskInfo->sourcePath << " ";
    qDebug() <<  "copyPath: " << taskInfo->copyPath << " ";
    qDebug() <<  "type: " << taskInfo->type << " ";
    qDebug() <<  "days: " << taskInfo->days << " ";
    qDebug() <<  "copyNum: " << taskInfo->copyNum << " ";
    qDebug() <<  "time: " << taskInfo->time.toString("hh:mm") << " ";
    qDebug() << "lasTRun: " << taskInfo->lastRun.toString();
    qDebug() << "lasComplite: " << taskInfo->lastComplite.toString();
    qDebug() << "proceed: " << taskInfo->proceed;
    qDebug();
    mutex->unlock();
}

void Task::createFileList()
{
    taskInfo->sourcePath;
}

QByteArray Task::getHash(QString _path)
{
    QFile file(_path);
    int bufSize = file.size();

    if(file.open(QFile::ReadOnly))
    {
        QCryptographicHash hash(QCryptographicHash::Sha1);
        while(!file.atEnd()){
            hash.addData(file.read(8192));
        }
        return hash.result();
    }
    return QByteArray();
}

void Task::run()
{
    mutex->lock();
    qDebug() << "Task " << taskInfo->name << " praparing to running in thread: " << QThread::currentThreadId();
    mutex->unlock();
    taskInfo->proceed = true;
    taskInfo->lastRun = QDateTime::currentDateTime();
    emit saveRegSignal();

    debugTaskInfo();
    waitTime();

    taskInfo->proceed = false;
    taskInfo->lastComplite = QDateTime::currentDateTime();
    qDebug() << "Task " << taskInfo->name << " complited";
    emit saveRegSignal();
}
