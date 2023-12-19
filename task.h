/*

Закончил зеркальное копирование
Проверить ещё раз в т.ч. удаление, перемещение, переименование, изменение

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
#include <QSet>
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
    QString backInTimePath; // В конфиге bitPath
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

    QSet<QString> sourceFiles; // Заполняется именами папок и файлов (без учёта указанного пути в информации о задании) при зеркальном копировании для дальнейшего анализа папки с копией

//    QMap<QByteArray, QString> sourceFilesHashList;
//    QMap<QString, QByteArray> sourceFilesList;
//    QMap<QString, QByteArray> sourceFilesListSaved;


//    void startTimer();
    void waitTime();

    void debugTaskInfo();

//    void createFileList(const QString &_path);
//    void analizeCopyFolder();
    QByteArray getHash(const QString &_path);


    // создаёт подпапки в указанной директории, принимает целевую директорию и путь, который надо создать в целевой директории
    void createDirs(const QString &_targetPath);

    void prepareCopyDir();
    void mirrorCopy(const QString &_path);
    void mirrorAnalyze(const QString &_path); // Проверяет на удаленные, перемещённые и переименованные файлы. Если файлы перемещены, они так же перемещаются в копии. Если удалены или переименованы - копии файлов отправялются в папку Back_In_Time

    void moveToBackInTime(QFileInfo _fileInfo, QString _sourcePath);

    // QRunnable interface
public:
    void run();
};

#endif // TASK_H
