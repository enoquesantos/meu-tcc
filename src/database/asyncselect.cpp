#include "asyncselect.h"
#include "database.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

AsyncSelect::AsyncSelect(const QString &tableName, const QStringList &jsonColumns, const QVariantMap &where, const QVariantMap &args, QObject *parent) :
    QThread(parent), m_tableName(tableName), m_jsonColumns(jsonColumns), m_where(where), m_args(args)
{
    m_database = Database::instance();
}

void AsyncSelect::run()
{
    if (m_tableName.isEmpty())
        return;
    QVariantList result(m_database->select(m_tableName, m_where, m_args));
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
        foreach (const QString &key, m_jsonColumns) {
            // try create a json document if data type is a valid json (from a string),
            // otherwise (is a plain string or number) continue to next key
            json = QJsonDocument::fromJson(map[key].toByteArray(), &jsonParseError);
            if (jsonParseError.error == QJsonParseError::NoError) {
                if (json.isObject() && !json.isEmpty())
                    map.insert(key, json.object().toVariantMap());
                else if (json.isArray() && !json.isEmpty())
                    map.insert(key, json.array().toVariantList());
            }
        }
        emit itemLoaded(map);
    }
}
