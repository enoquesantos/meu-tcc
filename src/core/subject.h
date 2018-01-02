#ifndef SUBJECT_H
#define SUBJECT_H

#include <QMap>
#include <QObject>
#include <QThread>
#include <QVector>
#include <QVariant>

#include "observer.h"
namespace Private {
/**
 * @brief The Worker class
 * This class will be instantiate for each observer to be updated in another application thread
 * This implementation is useful to optimize the process of notification for new events in a fast way
 * non-blocking the update while each observer execute your task.
 */
class Worker : public QThread
{
    Q_OBJECT
public:
    Worker(Observer *observer, const QString &event, const QVariant &data, QObject *sender, QObject *parent = nullptr) : QThread(parent)
    {
        m_observer = observer;
        m_event = event;
        m_data = data;
        m_sender = sender;
    }

private:
    /**
     * @brief m_data
     * The event data. Can be a integer, float, array or a object
     */
    QVariant m_data;

    /**
     * @brief m_event QString
     * The event name
     */
    QString m_event;

    /**
     * @brief m_observer Observer *
     * A pointer to the Observer
     */
    Observer *m_observer;

    /**
     * @brief m_sender QObject *
     * A pointer to event sender
     */
    QObject *m_sender;

protected:
    /**
     * @brief run
     * Execute the current thread operation, updating the observer from new event,
     * sending the arguments and the event sender.
     * This method is called internally, after call QThread.start();
     */
    void run() override
    {
        m_observer->update(m_event, m_data, m_sender);
    }
};
}

class Subject : public QObject
{
    Q_OBJECT
public:
    explicit Subject(QObject *parent = Q_NULLPTR);
    ~Subject();

    /**
     * @brief attach
     * Append a new observer to the observers list of the 'event' parameter. If observer is already on the list, will be ignored!
     * @param observer Observer * Observer* the observer. Can be any QML component
     * @param event QString the event name
     */
    Q_INVOKABLE void attach(Observer *observer, const QString &event);

    /**
     * @brief attach
     * Append a new observer to the observers list for each 'event' in the 'events' list parameter.
     * If observer is already on the list (for each event), will be ignored!
     * @param observer Observer* the observer. Can be any QML component
     * @param events QVariantList the event list where the observer will added
     */
    Q_INVOKABLE void attach(Observer *observer, const QStringList &events);

    /**
     * @brief dettach
     * Detach a observer of a event
     * @param observer Observer* the observer. Can be any QML component
     * @param event QString the event name
     */
    Q_INVOKABLE void detach(Observer *observer, const QString &event);

    /**
     * @brief dettach
     * Detach a observer from all events set in events parameter
     * @param observer Observer* the observer. Can be any QML component
     * @param event QString the event name
     */
    Q_INVOKABLE void detach(Observer *observer, const QStringList &events);

    /**
     * @brief notify
     * Notify all observers from new event, sending the event data as event argument and the event issuer.
     * @param event QString the event name
     * @param data QVariant the event data. Can be a integer, float, array or a object
     * @param sender QObject* the event issuer. Can be any QML component
     */
    Q_INVOKABLE void notify(const QString &event, const QVariant &data, QObject *sender);

private:
    /**
     * @brief m_attacheds
     * This map store a pointer of observer from each event used in application, like  evet_name -> Observers list.
     * The attach, detach and notify methods uses this object to append, remove and update the observers.
     */
    QMap<QString, QVector<Observer*>> m_attacheds;
};

#endif // SUBJECT_H
