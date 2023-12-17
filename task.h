#ifndef TASK_H
#define TASK_H

#include <QObject>
#include <QRunnable>
#include <QTimer>
#include <QDebug>
#include <QThread>
#include <QDateTime>
#include <QMutex>
#include <QDir>
#include <QFile>
#include <QMap>
#include <QCryptographicHash>

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

struct TaskInfo
{
    QString name;
    QString sourcePath;
    QString copyPath;
    int days;
    int type;
    int copyNum;
    QTime time;
    QDateTime lastRun;
    QDateTime lastComplite;
    bool proceed;
};

class Task : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit Task(TaskInfo *_taskInfo, QMutex *_mtx, QObject *parent = nullptr);

signals:
    void log(QString);
    void saveRegSignal();

public slots:
    void timerAlarm();

private:
    QTimer *timer;
    QMutex *mutex;
    TaskInfo *taskInfo;

    QMap<QString, QByteArray> filesList;

    void startTimer();
    void waitTime();

    void debugTaskInfo();

    void createFileList();
    void analizeCopyFolder();
    QByteArray getHash(QString _path);

    // QRunnable interface
public:
    void run();
};

#endif // TASK_H
