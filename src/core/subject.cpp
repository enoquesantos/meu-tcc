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

void Subject::attach(Observer *observer, const QVariantList &events)
{
    foreach (const QVariant &qv, events)
        attach(observer, qv.toString());
}

void Subject::dettach(Observer *observer, const QString &event)
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

void Subject::notify(const QString &event, const QVariant &args, QQuickItem *sender)
{
    QVector<Observer *> observers = m_attacheds.value(event);
    int size = observers.size();
    for (int i = 0; i < size; ++i) {
        Observer *obs = observers.at(i);
        if (obs && !obs->objectName().isEmpty()) {
            Private::Worker *worker = new Private::Worker(obs, event, args, sender, this);
            connect(worker, &QThread::finished, worker, &QObject::deleteLater);
            worker->start();
        }
    }
}
