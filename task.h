/*
Остановился на создании хешей для проверки перемещения или удаления файла.
Задуманная логика:
    во время основного прохода зеркального копирования
    проверяем текущий исходные файл на наличие его в списках, если его нет, добавляем.

    во время проверки зеркальной коппии на удалённые, перемещённые и переименованные файлы
    проверяем нет ли хеша этого файла в списке хешей, если есть - переносим копии по новому пути, если нет - значит файл был удалён, создаём его копию в папку времени


*/

#ifndef TASK_H
#define TASK_H

#include <QObject>
#include <QRunnable>
//#include <QTimer>
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
//    QTimer *timer;
    QMutex *mutex;
    TaskInfo *taskInfo;

    QMap<QByteArray, QString> sourceFilesHashList;
    QMap<QString, QByteArray> sourceFilesList;


//    void startTimer();
    void waitTime();

    void debugTaskInfo();

//    void createFileList(const QString &_path);
    void analizeCopyFolder();
    QByteArray getHash(const QString &_path);

    void prepareCopyDir();
    void mirrorCopy(const QString &_path);
    void mirrorAnalyze(const QString &_path); // Проверяет на удаленные, перемещённые и переименованные файлы. Если файлы перемещены, они так же перемещаются в копии. Если удалены или переименованы - копии файлов отправялются в папку Back_In_Time

    // QRunnable interface
public:
    void run();
};

#endif // TASK_H
