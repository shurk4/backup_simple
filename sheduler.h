#ifndef SHEDULER_H
#define SHEDULER_H

#include <QObject>
#include <QSettings>
#include <QDebug>
#include <QThread>
#include <QDateTime>
#include <QTimer>
#include <QThreadPool>
#include <QMutex>

#include "task.h"

class Sheduler : public QObject
{
    Q_OBJECT
public:
    explicit Sheduler(QObject *parent = nullptr);

    bool isReady();

    void debugSettings();

signals:
    void saveRegSignal();

public slots:
    void update();
    void saveRegSlot();
//    void runTask();

private slots:
    void timerAlarm();

private:
    QMap<QString, TaskInfo> tasks;

    QTimer *timer;
    void startTimer();

    QMutex mutex;

    bool ready = false;

    void updateSettings();
    void checkTasks();
    void runTask(QString name);
};

#endif // SHEDULER_H
