#ifndef OBSERVER_H
#define OBSERVER_H

#include <QObject>

class Subject;

/**
 * @brief The Observer class
 * This Observer class is a simple implementation of the Observer design pattern.
 * However, this class is more simple, where the observer can assign any of application events
 * and receive notifications in update method to notify qml components in the updated signal.
 *
 * This class is registered in the application as customContextProperties
 */
class Observer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString event READ event WRITE setEvent NOTIFY eventsChanged)
    Q_PROPERTY(QStringList events READ events WRITE setEvents NOTIFY eventsChanged)
public:
    /**
     * @brief Observer
     * The object construct
     * @param parent
     */
    explicit Observer(QObject *parent = nullptr);

    /**
     * Detach the object from all assigned events in the Subject
     */
    ~Observer();

    /**
     * @brief events
     * Return a list with all assigned events by this observer
     * @return QStringList
     */
    QStringList events();

    /**
     * @brief event
     * Return the last event from m_events list
     * @return QString
     */
    QString event();

    /**
     * @brief setSubject
     * Set the Subject pointer to m_subject list
     * m_subject is used in destructor to detach from all signed events
     * @param subject Subject*
     */
    void setSubject(Subject *subject);

    /**
     * @brief setEvents
     * Set a event list to m_events list
     * @param events QStringList
     */
    void setEvents(const QStringList &events);

    /**
     * @brief setEvent
     * Append a single event to m_events list
     * @param event QString
     */
    void setEvent(const QString &event);

    /**
     * @brief update
     * Set a event notification to observer instance, where reply with updated signal
     * @param eventName QString the name of event where the observer are notificated
     * @param eventData QVariant the event generic arguments. Can be a integer, string or QVariant[Map|List]
     */
    void update(const QString &eventName, const QVariant &eventData);

signals:
    /**
     * @brief eventsChanged
     * Notify the observer to events changes
     * @param events QStringList
     */
    void eventsChanged(const QStringList &events);

    /**
     * @brief updated
     * Notify the observer from new update from any of signed events
     * @param eventName QString
     * @param eventData QVariant
     */
    void updated(const QString &eventName, const QVariant &eventData);

private:
    /**
     * @brief m_events
     * A list with the names of signed events
     */
    QStringList m_events;

    /**
     * @brief m_subject
     * Keeps a pointer to Subject instance and
     * is used to detach the observer from all signed events
     */
    Subject *m_subject;
};

#endif // OBSERVER_H
