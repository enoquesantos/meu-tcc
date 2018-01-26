#ifndef NOTIFYDESKTOP_H
#define NOTIFYDESKTOP_H

#include <QObject>

class NotifyDesktop : public QObject
{
    Q_OBJECT
public:
    explicit NotifyDesktop(QObject *parent = nullptr);

signals:

public slots:
};

#endif // NOTIFYDESKTOP_H