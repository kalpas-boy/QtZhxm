#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include <QObject>

class database_manager : public QObject
{
    Q_OBJECT
public:
    explicit database_manager(QObject *parent = nullptr);

signals:
};

#endif // DATABASE_MANAGER_H
