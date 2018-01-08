#include "plugindatabasetablecreator.h"
#include "../database/database.h"

PluginDatabaseTableCreator::PluginDatabaseTableCreator(const QString &pluginSqlFilePath, QObject *parent) : QThread(parent)
{
    _database = Database::instance();
    _pluginSqlFilePath = pluginSqlFilePath;
}

void PluginDatabaseTableCreator::run()
{
    _database->createTable(_pluginSqlFilePath);
}
