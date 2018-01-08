#ifndef ASYNCSELECT_H
#define ASYNCSELECT_H

#include <QVariant>
#include <QThread>

class Database;

/**
 * @brief The AsyncSelect class
 * Execute a asynchronous selection in SQLITE database, using 'tableName', 'where' condition and 'args' as selection properties, like 'LIMIT' and 'OFFSET'.
 * The jsonColumns is needed if the table contains columns with serialized json strings, to the value are returned as QVariant(Map/List).
 * To retrieve the result set, the pointer send a 'itemLoaded' signal for each entry, with a QVariantMap as column_name->value.
 */
class AsyncSelect : public QThread {
    Q_OBJECT
public:
    /**
     * @brief AsyncSelect
     * @param tableName QString the table name to get the query selection
     * @param jsonColumns QStringList a list of columns names that has serialized json string to parse for map or array
     * @param where QVariantMap a map with columns_name->value to build the query filter condition
     * @param args the selection args to customize the query predicates
     * @param parent QObject* the object parent
     */
    explicit AsyncSelect(const QString &tableName, const QStringList &jsonColumns, const QVariantMap &where, const QVariantMap &args, QObject *parent = nullptr);

signals:
    /**
     * @brief itemLoaded
     * This signal will be emitted after load some database data (from select method).
     * @param data QVariantMap a map with column_name->value loaded from database.
     */
    void itemLoaded(const QVariantMap &data);

protected:
    /**
     * @brief run
     * @overload
     * The start point for the thread.
     * After call the thread->start(), the newly created thread calls this function.
     * @return void
     */
    void run() override;

private:
    /**
     * @brief m_tableName
     * The name of the table to select in Database
     */
    QString m_tableName;

    /**
     * @brief m_jsonColumns
     * The list of json columnns from current table, to be serialized as string before save
     * and parse to object or array before sent in 'itemLoaded' signal when select data.
     */
    QStringList m_jsonColumns;

    /**
     * @brief m_where
     * The predicates filter as object (javascript from QML),
     * the values needs to be: column_name->value.
     */
    QVariantMap m_where;

    /**
     * @brief m_args
     * The selection operators and custom arguments as object, like
     * the operator to compare, the limit of items or offset value (useful when paginate results).
     */
    QVariantMap m_args;

    /**
     * @brief m_database
     * A pointer to Database object where execute the selection.
     */
    Database *m_database;
};

#endif // ASYNCSELECT_H
