/*

Вынесено из mirroCopy


            если файл
            if(!sourceFilesList.contains(i.absoluteFilePath()))
            {
                QByteArray fileHash = getHash(i.absoluteFilePath());
                sourceFilesList.insert(i.absoluteFilePath(), getHash(i.absoluteFilePath()));
                if(!sourceFilesHashList.contains(fileHash))
                {
                    sourceFilesHashList.insert(fileHash, i.absoluteFilePath());
                }
                else
                {
                    // Добавить побитовое сравнение?

                }
            }
            else
            {
                if(1)
                {

                }
            }

*/

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
    qDebug() <<  "bitPath: " << taskInfo->backInTimePath << " ";
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
//    int bufSize = file.size();

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

void Task::createDirs(const QString &_targetPath)
{
    qDebug() << "\nCreate dirs";

    QStringList dirs = _targetPath.split(u'/');
    QString path = "";

    for(const auto &i : dirs)
    {
        if(i != "")
        {
            if(!i.contains(':'))
            {
                path += "/";
            }
            path += i;

            if(!QDir(path).exists())
            {
                qDebug() << "Dir not exist - create: " << path;

                if(QDir().mkdir(path))
                {
                    qDebug() << "Complited";
                    if(!QDir(path).exists())
                    {
                        qDebug() << "notExist! oO";
                    }
                }
                else qDebug() << "Copy error";
            }
        }
    }
    qDebug() << "All dirs created, path ready";
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
    qDebug() << "Mirror copy:";
    QDir sourceDir(_path);
    QFileInfoList infoList = sourceDir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);

    for(const auto &i : infoList)
    {
        if(i.isDir())
        {
            QString copySubDir(taskInfo->copyPath + i.absoluteFilePath().remove(taskInfo->sourcePath));
            if(!QDir(copySubDir).exists())
            {
                // Создание копии папки
                createDirs(copySubDir);
            }

            mirrorCopy(i.absoluteFilePath());
        }
        else
        {
            sourceFiles.insert(i.absoluteFilePath().remove(taskInfo->sourcePath)); // Запись файла в список для анализа копии
            QString sourceFile(i.absoluteFilePath());
            QString copyFile(taskInfo->copyPath + i.absoluteFilePath().remove(taskInfo->sourcePath));
            if(QFile(copyFile).exists())
            {
                qDebug() << "File " << copyFile << " is exist";
                if(i.lastModified() > QFileInfo(copyFile).lastModified())
                {
                    qDebug() << "\nFile is modified.";

                    moveToBackInTime(QFileInfo(copyFile), taskInfo->copyPath);

                    // Копирование основного файла в зеркало
                    QFile().copy(i.absoluteFilePath(), copyFile);
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
    qDebug() << "\nAnalyzing mirror. Current path: " << _path;
    QFileInfoList analizeList = QDir(_path).entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);

    for(const auto &i : analizeList)
    {
        qDebug() << i.absoluteFilePath();
        if(i.isDir())
        {
            mirrorAnalyze(i.absoluteFilePath());
        }
        else
        {
            if(!sourceFiles.contains(i.absoluteFilePath().remove(taskInfo->copyPath)))
            {
                qDebug() << "File is renamed, removed or deleted from source folder";
                moveToBackInTime(i, taskInfo->copyPath);
                QFile().remove(i.absoluteFilePath());
            }
        }
    }

    qDebug() << "Check dir isEmpty: " << _path;;
    if(QDir(_path).isEmpty())
    {
        qDebug() << "Dir is empty - deleted";
        QDir(_path).removeRecursively();
    }
}

void Task::moveToBackInTime(QFileInfo _fileInfo, QString _sourcePath)
{
    QString fileToCopy(_fileInfo.absoluteFilePath());
    // Создание папки времени
    QString bitPath = taskInfo->backInTimePath;

    // Создание пути
    bitPath += _fileInfo.absolutePath().remove(_sourcePath) + "/" + _fileInfo.fileName() + "/" + _fileInfo.lastModified().toLocalTime().toString("ddMMyyyy_hhmm");
    qDebug() << "mirrorCopyPath: " << bitPath;
    createDirs(bitPath);

    // Подготовка полного пути для доступа к файлу
    bitPath += "/" + _fileInfo.fileName();
    qDebug() << "mirrorCopyPath final: " << bitPath;

    // Копирование файла из копии в хранилище backInTime
    QFile().copy(fileToCopy, bitPath);
    // Удаление старой копии файла
    QFile().remove(fileToCopy);
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

//    prepareCopyDir(); // Походу не пригодится
    mirrorCopy(taskInfo->sourcePath);
    qDebug();
    mirrorAnalyze(taskInfo->copyPath);

    taskInfo->proceed = false;
    taskInfo->lastComplite = QDateTime::currentDateTime();
    qDebug() << "Task " << taskInfo->name << " complited";
    emit saveRegSignal();
}
