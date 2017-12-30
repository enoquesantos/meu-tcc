#ifndef SUBJECT_H
#define SUBJECT_H

#include <QMap>
#include <QObject>
#include <QQuickItem>
#include <QThread>
#include <QVector>

#include "observer.h"

namespace Private {
class Worker : public QThread
{
    Q_OBJECT
public:
    Worker(Observer *observer, const QString &event, const QVariant &args, QQuickItem *sender, QObject *parent = nullptr) : QThread(parent)
    {
        m_args = args;
        m_event = event;
        m_observer = observer;
        m_sender = sender;
    }

private:
    QVariant m_args;
    QString m_event;
    Observer *m_observer;
    QQuickItem *m_sender;

protected:
    void run() override
    {
        m_observer->update(m_event, m_args, m_sender);
    }
};
}

class Subject : public QObject
{
    Q_OBJECT
public:
    explicit Subject(QObject *parent = Q_NULLPTR);
    ~Subject();

    Q_INVOKABLE void attach(Observer *observer, const QString &event);

    Q_INVOKABLE void attach(Observer *observer, const QVariantList &events);

    Q_INVOKABLE void dettach(Observer *observer, const QString &event);

    Q_INVOKABLE void notify(const QString &event, const QVariant &args, QQuickItem *sender);

private:
    QMap<QString, QVector<Observer*>> m_attacheds;
};

#endif // SUBJECT_H
