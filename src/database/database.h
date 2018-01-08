#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>

class QByteArray;
class QDir;
class QFile;
class QSqlError;
class QSqlRecord;
class QStringList;
class QUrl;

/**
 * @brief The Database class
 * This class implements a Singleton pattern and manage database operations
 * with high level methods to select, update, insert and remove data into application database.
 * Supports only SQLITE and all operations is handle by QSqlDatabase and QSqlQuery with some customizations.
 */
class Database : public QObject
{
    Q_OBJECT
private:
    /**
     * @brief Database
     * The object construct
     * @param parent QObject*
     */
    explicit Database(QObject *parent = Q_NULLPTR);

    /**
     * @brief Database
     * In singleton object, the copy constructor needs to be private
     * @param other Database
     */
    Database(const Database &other);

    /**
     * @brief operator =
     * In singleton object, the operator '=' needs to be private
     */
    void operator=(const Database &);

    /**
     * @brief ~Database
     * The object destructor
     */
    virtual ~Database();

    /**
     * @brief openConnection
     * Open the first connection with database if is not yet opened.
     * This method too be create the ".db" file on the first object query execution.
     */
    void openConnection();

    /**
     * @brief setFileName
     * Set the absolute sqlite file name using the application writable location
     * and the QApplication::ApplicationName() to define and create the .db file.
     */
    void setFileName();

public:
    /**
     * @brief The SELECT_TYPE enum
     * This enumeration is used to decide when create a map from table
     * based on meta_key -> meta_value on resultSet method.
     */
    enum SELECT_TYPE
    {
        All_Itens_Int,
        Meta_Key_Value_Int
    };

    /**
     * @brief instance
     * Return the pointer to this object
     * @return Database*
     */
    static Database *instance();

    /**
     * @brief queryExec
     * Execute a generic sqlite query, like "SELECT * FROM application_settings"
     * To access the result content of the query, the public method QVariantList resultSet() can be used to interate in result set.
     * @param sqlQueryString QString
     * @return bool return true if the query was success executed, otherwise false.
     */
    bool queryExec(const QString &sqlQueryString);

    /**
     * @brief tableColumns
     * Return a list of table columns names as QStringList
     * from 'tableName' if exists in database. If the table not exists, return a empty list.
     * @param tableName QString the name of the table
     * @return QStringList
     */
    Q_INVOKABLE QStringList tableColumns(const QString &tableName);

    /**
     * @brief resultSet
     * Return a list of QVariantMap from the last query executed.
     * Each map on the list, contains the table properties as column name and the column value.
     * Can be used to retrieve result after some custom query with joins for example,
     * since the normal select(...) query does not support joins with different tables.
     * @param selectType integer SELECT_TYPE with All_Itens_Int default values
     * @return QVariantList of QVariantMap with column_name -> value
     */
    Q_INVOKABLE QVariantList resultSet(enum SELECT_TYPE selectType = All_Itens_Int);

    /**
     * @brief select
     * Execute a SQL select query in database and return the result set as QVariantList with QVariantMap's.
     * Each map on the list, contains the table properties as column name and the column value.
     * If the table uses meta_key->meta_value columns, you needs to pass 'selectType' in arg parameter with value '1', to 
     * to get the map's in resultset with meta_key as key and meta_value as value.
     * 
     * @param tableName QString the name of the table to execute the query
     * @param where QVariantMap a map with column name and column value to build the query predicates.
     * @param args QVariantMap a map with some extra arguments to filter the result set
     *
     * ATTENTION!
     * This method uses args parameter to build the query filter properties. The follow options can be passed is:
     * "limit"   -> integer with the SQL LIMIT of result set
     * "offset"  -> integer with the SQL OFFSET to start the result set list (useful to paginate data)
     * "order"   -> QString with the SQL "asc" or "desc" value to define the result set as descending or not
     * "orderBy" -> QString the column name to ordenate the result set
     * "whereOperator" -> QString the with "AND" or "OR" to set the predicates clauses comparison operator for 'where' map arguments
     * "whereComparator" -> QString with the value: "=" or "!=" or ">" or  "<" or "%LIKE%" and is used to set the predicates comparator value operator
     * "selectType" -> integer with value to construct the result set map. If the table uses "meta_key" -> "meta_value" columns.
     * in this case, set the "selectType" to 1 to get each map as table key and table value properties.
     *
     * @return QVariantList of QVariantMap with column_name -> value
     */
    Q_INVOKABLE QVariantList select(const QString &tableName, const QVariantMap &where = QVariantMap(), const QVariantMap &args = QVariantMap());

    /**
     * @brief insert
     * Execute a SQL insert operation in database into 'tableName' using a map with column_name->value.
     * This method uses addBindValue from QSqlQuery object to prepared statement, that prevent SQL injection or invalid data type.
     * @param tableName QString the table name to save insertData data parameter in database
     * @param insertData QVariantMap a map with column_name -> value to insert in database
     * @return int return the insert ID if table has primary key ID column.
     */
    Q_INVOKABLE int insert(const QString &tableName, const QVariantMap &insertData);

    /**
     * @brief remove
     * Execute a simple SQL delete operation in database, using the tableName with where map + operators to build the string query
     * @param tableName QString the table name to delete data
     * @param where QVariantMap a map with data to be deleted
     * @param whereComparator QString the operator to use as compare operator at where clausule in query string
     * @return
     */
    Q_INVOKABLE int remove(const QString &tableName, const QVariantMap &where, const QString &whereComparator = QStringLiteral("="));

    /**
     * @brief update
     * Execute a SQL update operation in database, using the tableName and updateData to save based on where predicates.
     * @param tableName QString the table name to update data
     * @param updateData QVariantMap the map with column name -> value to save in database
     * @param where QVariantMap the map with column name -> condition value to filter the operation
     * @param whereOperator QString the operator to join where properties values. The possible values is "AND" or "OR". The default is "AND"
     * @param whereComparator QString the operator to use in comparison predicates. The possible values is "=", "!=", ">", "<", "<=", ">=" or "%LIKE%"
     * @return int return number of rows affected if success, otherwise return 0 (zero) (if occour any error).
     * To get the error details, create a connection with logMessage signal or call lastError().
     */
    Q_INVOKABLE int update(const QString &tableName, const QVariantMap &updateData, const QVariantMap &where, const QString &whereOperator = QStringLiteral("AND"), const QString &whereComparator = QStringLiteral("="));

    /**
     * @brief lastInsertId
     * this method can be used to retrieve the last insert id. Uses QSqlQuery::lastInsertId().
     *
     * @WARNING!
     * lastInsertId() return value that have been auto-generated by the autoincremented column!
     * if last insert occour in any table that not has autoincremented column the returned value will be zero!
     * @return int
     */
    Q_INVOKABLE int lastInsertId() const;

    /**
     * @brief numRowsAffected
     * return the number of rows affected after update or insert method. Uses the QSqlQuery::numRowsAffected() method.
     * @return int
     */
    Q_INVOKABLE int numRowsAffected() const;

    /**
     * @brief lastRowId
     * Retieve the SQLITE ROWID value. In some cases or tables, is not reffer to the primary key "id".
     * @param tableName QString the table name
     * @return int if table name is empty or can't ready the table last ROWID return 0. Otherwise the last row ID! :)
     */
    Q_INVOKABLE int lastRowId(const QString &tableName);

    /**
     * @brief createTable
     * This method ready a ".sql" file pass in filePath parameter.
     * Will be create a string query and execute using queryExec(QString&) to create the database table.
     *
     * @WARNING!
     * Any internal database error for the file will be ignored!
     * Your needs to prevent any errors from the SQL commands into this file.
     * @param filePath QString the path to ".sql" file in operating system.
     */
    void createTable(const QString &filePath);

    /**
     * @brief lastQuery
     * get the last query executed in database. Uses the QSqlQuery::lastQuery().
     * @return QString a empty string if the last query is none. Otherwise, return the last string query.
     */
    QString lastQuery() const;

    /**
     * @brief lastError
     * @return QString a empty string if the last query is success executed. Otherwise, return the last query error message
     */
    QString lastError() const;

signals:
    /**
     * @brief logMessage
     * Send operation messages, like errors and success queries
     * @param message QString a string with the generic messages
     */
    void logMessage(const QString &message);

private:
    /**
     * @brief m_instance
     * keeps the Database instance pointer
     */
    static Database *m_instance;

    /**
     * @brief m_databaseFileName
     * Keeps the absolute SQLITE .db file path and name.
     * This string is buil based on the platform.
     */
    QString m_databaseFileName;

    /**
     * @brief m_qsqlQuery
     * This object handle SQL DML operations executing the queries operations.
     * The QSqlQuery class provides a means of executing and manipulating SQL statements.
     * QSqlQuery encapsulates the functionality involved in creating, navigating and
     * retrieving data from SQL queries which are executed on a QSqlDatabase.
     */
    QSqlQuery m_qsqlQuery;

    /**
     * @brief m_qSqlDatabase
     * This object handle SQLITE database creation and connections.
     * The QSqlDatabase class provides an interface for accessing a database through a connection.
     * An instance of QSqlDatabase represents the connection,
     * providing access to the database via one of the supported database drivers,
     * which are derived from QSqlDriver.
     */
    QSqlDatabase m_qSqlDatabase;
};

#endif // DATABASE_H
