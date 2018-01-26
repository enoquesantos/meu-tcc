#ifndef NOTIFYIOS_H
#define NOTIFYIOS_H

#include <QObject>

class NotifyIOS : public QObject
{
    Q_OBJECT
public:
    explicit NotifyIOS(QObject *parent = nullptr);

signals:

public slots:
};

#endif // NOTIFYIOS_H