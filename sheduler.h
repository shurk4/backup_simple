#ifndef SHEDULER_H
#define SHEDULER_H

#include <QObject>
#include <QSettings>
#include <QDebug>
#include <QThread>

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
};

class Sheduler : public QObject
{
    Q_OBJECT
public:
    explicit Sheduler(QObject *parent = nullptr);

    bool isReady();

    void debugSettings();

signals:

public slots:
    void update();
//    void runTask();

private:
    QMap<QString, TaskInfo> tasks;

    bool ready = false;

    void updateSettings();

};

#endif // SHEDULER_H
