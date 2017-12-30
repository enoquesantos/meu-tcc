#include "observer.h"
#include "subject.h"

Observer::Observer(QObject *parent) : QObject(parent)
{
}

Observer::~Observer()
{
    if (m_subject && m_events.size()) {
        foreach (const QString &event, m_events)
            m_subject->dettach(this, event);
    }
}

QStringList Observer::events()
{
    return m_events;
}

void Observer::setSubject(Subject *subject)
{
    m_subject = subject;
}

void Observer::setEvents(const QStringList &events)
{
    m_events = events;
    emit eventsChanged(m_events);
}

void Observer::update(const QString &eventName, const QVariant &args, QQuickItem *sender)
{
    // will be execute in the same thread of Worker thread
    emit updated(eventName, args, sender);
}
