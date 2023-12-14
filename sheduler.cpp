#include "sheduler.h"

Sheduler::Sheduler(QObject *parent)
    : QObject{parent}
{
    updateSettings();
    debugSettings();
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
        qDebug() <<  "sourcePath: " << i.sourcePath << " ";
        qDebug() <<  "copyPath: " << i.copyPath << " ";
        qDebug() <<  "type: " << i.type << " ";
        qDebug() <<  "days: " << i.days << " ";
        qDebug() <<  "copyNum: " << i.copyNum << " ";
        qDebug();
    }
}

void Sheduler::update()
{
    updateSettings();
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
        tasks[i].days = settings.value("days").toInt();
        tasks[i].type = settings.value("type").toInt();
        tasks[i].copyNum = settings.value("copysNum").toInt();

        settings.endGroup();
    }
    settings.endGroup();
    ready = true;
}
