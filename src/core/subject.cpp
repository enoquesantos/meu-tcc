#include "subject.h"

#include <QMapIterator>

Subject::Subject(QObject *parent) : QObject(parent)
{
}

Subject::~Subject()
{
    QMapIterator<QString, QVector<Observer *>> i(m_attacheds);
    while (i.hasNext()) {
      i.next();
      QVector<Observer *> list = i.value();
        while (!list.isEmpty())
            delete list.takeFirst();
    }
    m_attacheds.clear();
}

void Subject::attach(Observer *observer, const QString &event)
{
    // if observer not set the object name cannot be attached to subject
    if (!observer || observer->objectName().isEmpty())
        return;
    QVector<Observer *> attacheds = m_attacheds.value(event);
    if (attacheds.count() > 0) {
        int index = attacheds.indexOf(observer);
        if (index > -1)
            attacheds.removeAt(index);
    }
    observer->setSubject(this);
    attacheds.append(observer);
    m_attacheds.insert(event, attacheds);
}

void Subject::attach(Observer *observer, const QStringList &events)
{
    foreach (const QString &event, events)
        attach(observer, event);
}

void Subject::detach(Observer *observer, const QString &event)
{
    QVector<Observer *> attacheds = m_attacheds.value(event);
    if (attacheds.count() > 0) {
        int index = attacheds.indexOf(observer);
        if (index > -1)
            attacheds.removeAt(index);
    }
    m_attacheds.insert(event, attacheds);
    attacheds = m_attacheds.value(event);
}

void Subject::detach(Observer *observer, const QStringList &events)
{
    foreach (const QString &event, events)
        detach(observer, event);
}

void Subject::notify(const QString &event, const QVariant &data, QObject *sender)
{
    QVector<Observer *> observers = m_attacheds.value(event);
    int size = observers.size();
    for (int i = 0; i < size; ++i) {
        Observer *obs = observers.at(i);
        if (obs && !obs->objectName().isEmpty()) {
            Private::Worker *worker = new Private::Worker(obs, event, data, sender, this);
            connect(worker, &QThread::finished, worker, &QObject::deleteLater);
            worker->start();
        }
    }
}
