#ifndef OBSERVER_H
#define OBSERVER_H

#include <QObject>
#include <QQuickItem>

class Subject;

class Observer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList events READ events WRITE setEvents NOTIFY eventsChanged)
public:
    explicit Observer(QObject *parent = Q_NULLPTR);
    ~Observer();
    QStringList events();
    void setSubject(Subject *subject);
    void setEvents(const QStringList &events);
    void update(const QString &eventName, const QVariant &args, QQuickItem *sender);

signals:
    void eventsChanged(const QStringList &events);
    void updated(const QString &eventName, const QVariant &args, QQuickItem *sender);

private:
    QStringList m_events;
    Subject *m_subject;
};

#endif // OBSERVER_H
