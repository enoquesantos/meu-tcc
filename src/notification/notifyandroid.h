#ifndef NOTIFYANDROID_H
#define NOTIFYANDROID_H

#include <QObject>

class NotifyAndroid : public QObject
{
    Q_OBJECT
public:
    explicit NotifyAndroid(QObject *parent = nullptr);

signals:

public slots:
};

#endif // NOTIFYANDROID_H