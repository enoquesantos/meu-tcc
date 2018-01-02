#include "observer.h"
#include "subject.h"

Observer::Observer(QObject *parent) : QObject(parent)
{
}

Observer::~Observer()
{
    if (m_subject && m_events.size())
        m_subject->detach(this, m_events);
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

void Observer::update(const QString &eventName, const QVariant &eventData, QObject *sender)
{
    emit updated(eventName, eventData, sender);
}
