#include "databasecomponent.h"
#include "database.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

Private::AsyncSelect::AsyncSelect(QString *tableName, QStringList *jsonColumns, const QVariantMap &where, const QVariantMap &args, QObject *parent) :
    QThread(parent), m_tableName(tableName), m_jsonColumns(jsonColumns), m_where(where), m_args(args)
{
    m_database = Database::instance();
}

void Private::AsyncSelect::run()
{
    if (m_tableName->isEmpty())
        return;
    QVariantList result(m_database->select(*(m_tableName), m_where, m_args));
    int resultSize = result.size();
    if (!resultSize)
        return;
    QVariantMap map;
    QJsonDocument json;
    QJsonParseError jsonParseError;
    // iterate in all entries to parse json objects if exists
    for (int i = 0; i < resultSize; ++i) {
        map = result[i].toMap();
        // iterate the map to try parse property to object or array
        // if was saved as string. This is needed to qml receive as array or object
        // and prevent qml objects to make JSON.parse(...)
        // JSON.parse can't parse deep json arrays or objects!
        foreach (const QString &key, *(m_jsonColumns)) {
            // try create a json document if data type is a valid json (from a string),
            // otherwise (is a plain string or number) continue to next key
            json = QJsonDocument::fromJson(map[key].toByteArray(), &jsonParseError);
            if (jsonParseError.error == QJsonParseError::NoError) {
                if (json.isObject())
                    map.insert(key, json.object().toVariantMap());
                else if (json.isArray())
                    map.insert(key, json.array().toVariantList());
            }
        }
        emit itemLoaded(map);
    }
}

DatabaseComponent::DatabaseComponent(QObject *parent) : QObject(parent)
  ,m_totalItens(0)
  ,m_database(Database::instance())
{
}

void DatabaseComponent::load()
{
    // load the table columns names
    m_tableColumns = m_database->tableColumns(m_tableName);

    // uses the generic query execution and load the result with 'resultSet' method
    m_database->queryExec(QStringLiteral("select id from ") + m_tableName);
    QVariantList result(m_database->resultSet());

    // keeps the number of saved itens
    m_totalItens = result.size();
    // if total entries is zero or above 500, we cannot set
    // the table keys to 'm_savedPks' to optimize the memory consumption!
    if (!m_totalItens || m_totalItens > 1000)
        return;

    // set all primary keys that can be used to
    // check if some data is already in database
    QString pkColumn(QStringLiteral("id"));
    if (!m_pkColumn.isEmpty())
        pkColumn = m_pkColumn;
    for (int i = 0; i < m_totalItens; ++i)
        m_savedPks << result[i].toMap().value(pkColumn);
}

void DatabaseComponent::setTableName(const QString &tableName)
{
    m_tableName = tableName;
    if (!m_tableName.isEmpty())
        load();
}

void DatabaseComponent::setPkColumn(const QString &pkColumn)
{
    m_pkColumn = pkColumn;
}

void DatabaseComponent::setJsonColumns(const QStringList &jsonColumns)
{
    m_jsonColumns = jsonColumns;
}

bool DatabaseComponent::containsId(const QVariant &item)
{
    return m_savedPks.contains(item);
}

void DatabaseComponent::parseData(QVariantMap *data)
{
    // iterate in all table columns
    // removing all data that is not on the table!
    foreach (const QString &column, m_tableColumns) {
        if (!data->contains(column))
            data->remove(column);
        if (data->value(column).userType() == QMetaType::QVariantMap) {
            // if column data has a json object, parse from map to minified string
            QJsonDocument doc(QJsonObject::fromVariantMap(data->value(column).toMap()));
            data->insert(column, QVariant(doc.toJson(QJsonDocument::Compact)));
        } else if (data->value(column).userType() == QMetaType::QVariantList) {
            // if column data has a json array, parse from list to minified string
            QJsonDocument doc(QJsonArray::fromVariantList(data->value(column).toList()));
            data->insert(column, QVariant(doc.toJson(QJsonDocument::Compact)));
        }
    }
}

int DatabaseComponent::insert(const QVariantMap &data)
{
    if (m_tableName.isEmpty())
        return 0;
    QString pkColumn(QStringLiteral("id"));
    if (!m_pkColumn.isEmpty())
        pkColumn = m_pkColumn;
    QVariant id = data.value(pkColumn);
    if (m_tableName.isEmpty() || (id.isValid() && m_savedPks.size() && m_savedPks.contains(id)))
        return 0;
    QVariantMap insertData(data);
    parseData(&insertData);
    int insertId = m_database->insert(m_tableName, insertData);
    if (insertData.size() && insertId) {
        m_savedPks << id;
        m_totalItens = m_savedPks.size();
    }
    return insertId;
}

void DatabaseComponent::select(const QVariantMap &where, const QVariantMap &args)
{
    // make a asynchronous selection in database, using another thread created by Private::AsyncSelect
    auto *worker = new Private::AsyncSelect(&m_tableName, &m_jsonColumns, where, args, this);

    // after thread finished, delete the worker pointer
    connect(worker, &QThread::finished, worker, &QObject::deleteLater);

    // after worker thread load all result, we needs to emit the itemLoaded signal to QML object
    connect(worker, &Private::AsyncSelect::itemLoaded, this, &DatabaseComponent::itemLoaded);

    // start thread execution, internally call the 'run' method, where is the selection in database
    worker->start();
}

int DatabaseComponent::update(const QVariantMap &data, const QVariantMap &where)
{
    if (m_tableName.isEmpty())
        return 0;
    QVariantMap updateData(data);
    parseData(&updateData);
    int updateId = m_database->update(m_tableName, updateData, where);
    if (updateData.size() && updateId)
        return updateId;
    return 0;
}

int DatabaseComponent::remove(const QVariantMap &where)
{
    if (m_tableName.isEmpty() || !where.size())
        return 0;
   return m_database->remove(m_tableName, where);
}
