#ifndef PLUGINDATABASETABLECREATOR_H
#define PLUGINDATABASETABLECREATOR_H

#include <QThread>

class Database;

/**
 * @brief The PluginDatabaseCreator class
 * @extends QThread
 * This class create a plugin database table in application SQLITE database, executing in background thread.
 * The table file can contains a table creation or update rules.
 * A instance of this class is created for each plugin that contains the 'plugin_table.sql' file.
 */
class PluginDatabaseTableCreator : public QThread
{
    Q_OBJECT
public:
    /**
     * @brief PluginDatabaseCreator
     * @param pluginSqlFilePath QString a string with absolute path to 'plugin_table.sql' file
     * @param parent QObject* the parent of this instance
     */
    PluginDatabaseTableCreator(const QString &pluginSqlFilePath, QObject *parent = nullptr);

protected:
    /**
     * @brief run
     * @overload
     * Start the thread execution
     * @return void
     */
    void run() override;

private:
    Database *_database;
    QString _pluginSqlFilePath;
};

#endif // PLUGINDATABASETABLECREATOR_H
