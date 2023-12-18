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

//void Task::startTimer()
//{
//    timer = new QTimer();
//    connect(timer, &QTimer::timeout, this, &Task::timerAlarm);
//    timer->start(800);
//}

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

//void Task::createFileList(const QString &_path)
//{
//    qDebug() << "Crate file list";
//    QDir sourceDir(_path);
//    QFileInfoList infoList = sourceDir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);

//    for(const auto &i : infoList)
//    {
//        if(i.isDir())
//        {
//            createFileList(i.absoluteFilePath());
//        }
//        else
//        {
//            qDebug() << i.absoluteFilePath();
//            qDebug() << i.absoluteFilePath().remove(taskInfo->sourcePath);
//            qDebug() << taskInfo->copyPath + i.absoluteFilePath().remove(taskInfo->sourcePath);
//            qDebug();
//        }
//    }
//}

QByteArray Task::getHash(const QString &_path)
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

void Task::prepareCopyDir()
{
    if(!QDir(taskInfo->copyPath + "/mirror").exists())
    {
        QDir().mkdir(taskInfo->copyPath + "/mirror");
    }

    if(!QDir(taskInfo->copyPath + "/Back_In_Time").exists())
    {
        QDir().mkdir(taskInfo->copyPath + "/Back_In_Time");
    }
}

void Task::mirrorCopy(const QString &_path)
{
    qDebug() << "Mirror copy mirrorCopy:";
    QDir sourceDir(_path);
    QFileInfoList infoList = sourceDir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);

    for(const auto &i : infoList)
    {
        if(i.isDir())
        {
            QString copySubDir(taskInfo->copyPath+ "/mirror" + i.absoluteFilePath().remove(taskInfo->sourcePath));
            if(!QDir(copySubDir).exists())
            {
                QDir().mkdir(copySubDir);
                qDebug() << "Dir not exist - created!";
            }

            mirrorCopy(i.absoluteFilePath());
        }
        else
        {
            if(!sourceFilesList.contains(i.absoluteFilePath()))
            {
                sourceFilesHashList[getHash(i.absoluteFilePath())] = i.absoluteFilePath();
                sourceFilesList[i.absoluteFilePath()] = getHash(i.absoluteFilePath());
            }

            QString sourceFile(i.absoluteFilePath());
            QString copyFile(taskInfo->copyPath + "/mirror" + i.absoluteFilePath().remove(taskInfo->sourcePath));
            if(QFile(copyFile).exists())
            {
                qDebug() << "File " << copyFile << " is exist";
                if(i.lastModified() > QFileInfo(copyFile).lastModified())
                {
                    // Folder with file name
                    QString mirrorCopyPath = taskInfo->copyPath + "/Back_In_Time/" + i.fileName();
                    if(!QDir(mirrorCopyPath).exists())
                    {
                        // !!! УДАЛИТЬ СТАРЫЕ ЛИШНИЕ КОПИИ. КОЛИЧЕСТВО ХРАНИМЫХ КОПИЙ БРАТЬ ИЗ ПЕРЕМЕННОЙ КОЛИЧЕСТВО КОПИЙ

                        QDir().mkdir(mirrorCopyPath);
                    }

                    // Folder with change date
                    mirrorCopyPath += "/" + i.lastModified().toLocalTime().toString("ddMMyyyy_hhmm");
                    if(!QDir(mirrorCopyPath).exists())
                    {
                        // !!! УДАЛИТЬ СТАРЫЕ ЛИШНИЕ КОПИИ. КОЛИЧЕСТВО ХРАНИМЫХ КОПИЙ БРАТЬ ИЗ ПЕРЕМЕННОЙ КОЛИЧЕСТВО КОПИЙ

                        QDir().mkdir(mirrorCopyPath);
                    }

                    mirrorCopyPath += "/" + i.fileName();

                    // Копирование файла из копии в хранилище Back_in_time
                    QFile().copy(copyFile, mirrorCopyPath);
                    // Удаление старой копии файла
                    QFile().remove(copyFile);
                    // Копирование основного файла в зеркало
                    QFile().copy(sourceFile, copyFile);
                }
            }
            else
            {
                QFile().copy(sourceFile, copyFile);
                qDebug() << "File copy is created!";
            }
        }
    }
}

void Task::mirrorAnalyze(const QString &_path)
{
    qDebug() << "Analyzing mirror";
    QFileInfoList fileList = QDir(_path).entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);

    for(const auto &i : fileList)
    {
        if(i.isDir())
        {
            QString sourcePath = taskInfo->sourcePath + i.absoluteFilePath().remove(taskInfo->copyPath + "/mirror");

            qDebug() << "Source of copy path: " << sourcePath;

            if(!QDir(sourcePath).exists())
            {
//                QDir(taskInfo->sourcePath).
            }
            mirrorAnalyze(i.absoluteFilePath());
        }
    }
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

    prepareCopyDir();
    mirrorCopy(taskInfo->sourcePath);
    mirrorAnalyze(taskInfo->copyPath + "/mirror");

    taskInfo->proceed = false;
    taskInfo->lastComplite = QDateTime::currentDateTime();
    qDebug() << "Task " << taskInfo->name << " complited";
    emit saveRegSignal();
}
