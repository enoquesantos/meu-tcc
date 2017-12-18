#include "database.h"

#include <QApplication>
#include <QByteArray>
#include <QDir>
#include <QFile>
#include <QSqlError>
#include <QSqlRecord>
#include <QStandardPaths>
#include <QStringList>
#include <QUrl>

#ifdef QT_DEBUG
#include <QDebug>
#endif

Database* Database::m_instance = nullptr;

Database::Database(QObject *parent) : QObject(parent)
{
    setFileName();
#ifdef QT_DEBUG
    connect(this, &Database::logMessage, [this](const QString &message) {
        qDebug() << message;
    });
#endif
}

Database::~Database()
{
    if (m_qSqlDatabase.isOpen())
        m_qSqlDatabase.close();
}

Database *Database::instance()
{
    if (!Database::m_instance) {
        Database::m_instance = new Database;
        Database::m_instance->logMessage(QStringLiteral("Database instance created!'"));
    }
    return Database::m_instance;
}

QStringList Database::tableColumns(const QString &tableName)
{
    QStringList list;
    if (tableName.isEmpty())
        return list;
    queryExec(QStringLiteral("PRAGMA table_info(") + tableName + QStringLiteral(");"));
    QVariantList result(resultSet());
    foreach (const QVariant &item, result)
        list << item.toMap().value(QStringLiteral("name")).toString();
    if (list.size())
        emit logMessage(QStringLiteral("load '") + tableName + QStringLiteral("' columns names!"));
    else
        emit logMessage(QStringLiteral("Error! The table '") + tableName + QStringLiteral("' not exists on database!"));
    return list;
}

QVariantList Database::resultSet(enum SELECT_TYPE selectType)
{
    int i = 0;
    int totalColumns = 0;
    QSqlRecord record;
    QVariantMap map;
    QVariantList resultSet;

    while (m_qsqlQuery.next()) {
        record = m_qsqlQuery.record();
        if (record.isEmpty() || !m_qsqlQuery.isValid())
            continue;
        totalColumns = record.count();

        for (i = 0; i < totalColumns; ++i) {
            if (selectType == All_Itens_Int)
                map.insert(record.fieldName(i), m_qsqlQuery.value(i));
            else if (selectType == Meta_Key_Value_Int)
                map.insert(m_qsqlQuery.value(0).toString(), m_qsqlQuery.value(1));
        }

        if (!map.isEmpty())
            resultSet.append(map);
    }
    return resultSet;
}

void Database::openConnection()
{
    QString applicationName(QApplication::applicationName());
    if (!m_qSqlDatabase.isOpen()) {
        if (!QSqlDatabase::contains(applicationName)) {
            m_qSqlDatabase = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), applicationName);
            m_qSqlDatabase.setDatabaseName(m_databaseFileName);
        }
        m_qSqlDatabase = QSqlDatabase::database(applicationName);
        m_qsqlQuery = QSqlQuery(m_qSqlDatabase);
    }
    if (!m_qSqlDatabase.isOpen())
        emit logMessage(QStringLiteral("Fatal error on init database! Connection cannot be opened!"));
}

void Database::createTable(const QString &filePath)
{
    QFile file(filePath);
    if (!file.exists()) {
        emit logMessage(QStringLiteral("Fatal error on build database. The file '") + file.fileName() + QStringLiteral("' cannot be not found!"));
        return;
    } else if (!file.open(QIODevice::ReadOnly)) {
        emit logMessage(QStringLiteral("Fatal error on try to create table! The file '") + file.fileName() + QStringLiteral("' cannot be opened!"));
        return;
    }

    bool hasText;
    QString line;
    QByteArray readLine;
    QString cleanedLine;
    QStringList strings;

    while (!file.atEnd()) {
        hasText     = false;
        line        = "";
        readLine    = "";
        cleanedLine = "";
        strings.clear();
        while (!hasText) {
            readLine    = file.readLine();
            cleanedLine = readLine.trimmed();
            strings     = cleanedLine.split("--");
            cleanedLine = strings.at(0);
            if (!cleanedLine.startsWith("--") && !cleanedLine.isEmpty())
                line += cleanedLine;
            if (cleanedLine.endsWith(";"))
                break;
            if (cleanedLine.startsWith("COMMIT"))
                hasText = true;
        }
        if (!line.isEmpty())
            queryExec(line);
        if (!m_qsqlQuery.isActive())
            emit logMessage(m_qsqlQuery.lastError().text());
    }
    file.close();
    QFile::setPermissions(m_databaseFileName, QFile::WriteOwner | QFile::ReadGroup | QFile::ReadUser | QFile::ReadOther);
    emit logMessage(QStringLiteral("Database table for '") + filePath + QStringLiteral("' created!"));
}

void Database::setFileName()
{
    // set the absolute path for the sqlite file (using the application writeable directory).
    // the m_databaseFileName define to: writeable-directory-path + the-application-name + .db
    QString path;
#if defined(Q_OS_IOS)
    path = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/";
#else
    // android or desktop (linux|osx)
    path = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/";
#endif
    QDir dir(path);
    if (!dir.exists())
        dir.mkpath(path);
    m_databaseFileName = path + QApplication::applicationName() + QStringLiteral(".db");
    emit logMessage(QStringLiteral("Database absolute name defined to: ") + m_databaseFileName);
}

bool Database::queryExec(const QString &query)
{
    openConnection();

    if (m_qsqlQuery.exec(query)) {
        emit logMessage(QStringLiteral("Query success executed: ") + query);
        return true;
    }

    if (m_qsqlQuery.lastError().type() != QSqlError::NoError)
        emit logMessage(QStringLiteral("Fatal error on try execute query: ") + lastError());

    return false;
}

QVariantList Database::select(const QString &tableName, const QVariantMap &where, const QVariantMap &args)
{
    QVariantList resultSet;
    if (tableName.isEmpty()) {
        emit logMessage(QStringLiteral("Fatal error on try select data: The table name is empty!"));
        return resultSet;
    }

    openConnection();
    if (!m_qSqlDatabase.isOpen()) {
        emit logMessage(QStringLiteral("Fatal error on try select: Database connection cannot be opened!"));
        return resultSet;
    }

    int limit = args.value(QStringLiteral("limit"), -1).toInt();
    int offset = args.value(QStringLiteral("offset"), 0).toInt();
    QString order = args.value(QStringLiteral("order"), QStringLiteral("asc")).toString();
    QString orderBy = args.value(QStringLiteral("orderby"), QStringLiteral("")).toString();
    QString whereOperator = args.value(QStringLiteral("whereOperator"), QStringLiteral("AND")).toString();
    QString whereComparator = args.value(QStringLiteral("whereOperator"), QStringLiteral("=")).toString();
    SELECT_TYPE selectType = static_cast<SELECT_TYPE>(args.value(QStringLiteral("selectType"), SELECT_TYPE::All_Itens_Int).toInt());
    bool withLikeClause = (whereComparator.compare(QStringLiteral("LIKE"), Qt::CaseInsensitive) == 0);

    QString whereStr;
    QString query(QStringLiteral("select * from ") + tableName);

    if (!where.isEmpty()) {
        int k = 0;
        foreach (const QString &key, where.keys())
            whereStr.append(QString(QStringLiteral("%1 %2 %3 %4")).arg(k++ == 0 ? "" : " " + whereOperator, key, whereComparator, "?"));
        query.append(QStringLiteral(" where ")).append(whereStr.remove("  ").trimmed().simplified());
    }

    if (!orderBy.isEmpty()) {
        query.append(QString(QStringLiteral(" order by %1")).arg(orderBy));
        if (!orderBy.contains(QStringLiteral("asc"), Qt::CaseInsensitive) || orderBy.contains(QStringLiteral("desc"), Qt::CaseInsensitive))
            query.append(QStringLiteral(" ") + order);
    }

    if (limit > 0 && offset == 0)
        query.append(QStringLiteral(" limit ") + QString::number(limit));
    else if (offset > 0)
        query.append(QStringLiteral(" limit ") + QString::number(limit) + QStringLiteral(" offset ") + QString::number(offset));

    m_qsqlQuery.prepare(query);

    foreach (const QString &key, where.keys())
        m_qsqlQuery.addBindValue(withLikeClause ? QVariant("\%"+where.value(key).toString()+"\%") : where.value(key));

    if (!m_qsqlQuery.exec())
        return resultSet;

    emit logMessage(QStringLiteral("Query success executed: ") + query);

    int totalColumns = m_qsqlQuery.record().count();
    if (!totalColumns || !m_qsqlQuery.size())
        return resultSet;

    return this->resultSet(selectType);
}

int Database::insert(const QString &tableName, const QVariantMap &insertData)
{
    if (tableName.isEmpty()) {
        emit logMessage(QStringLiteral("Fatal error on try insert: The table name is empty!"));
        return 0;
    } else if (insertData.isEmpty()) {
        emit logMessage(QStringLiteral("Fatal error on try insert: The insertData is empty!"));
        return 0;
    }

    QStringList strValues;
    QStringList fields(insertData.keys());
    QVariantList values(insertData.values());
    int totalFields = fields.size();
    for (int i = 0; i < totalFields; ++i)
        strValues.append("?");

    openConnection();

    QString query("INSERT INTO " + tableName + "(" + QString(fields.join(",")) + ") VALUES(" + QString(strValues.join(",")) + ")");
    m_qsqlQuery.prepare(query);

    int k = 0;
    foreach (const QVariant &value, values)
        m_qsqlQuery.bindValue(k++, value);

    if (m_qsqlQuery.exec())
        return lastInsertId();

    QString error(lastError());
    if (!error.isEmpty()) {
        error.prepend(QStringLiteral("Fatal error on try insert: "));
        emit logMessage(error);
    }

    return 0;
}

int Database::remove(const QString &tableName, const QVariantMap &where, const QString &whereComparator)
{
    if (tableName.isEmpty()) {
        emit logMessage(QStringLiteral("Fatal error on try remove item: The table name is empty!"));
        return 0;
    } else if (where.isEmpty()) {
        emit logMessage(QStringLiteral("Fatal error on try remove item: The where parameter is empty!"));
        return 0;
    }

    int k = 0;
    QString whereStr;
    QString separator;

    QMap<QString, QVariant>::const_iterator j = where.constBegin();
    while (j != where.constEnd()) {
        separator = (k++ == 0) ? QStringLiteral("") : QStringLiteral(" AND ");
        whereStr += QString("%1%2 %3 '%4'").arg(separator).arg(j.key()).arg(whereComparator).arg(j.value().toString());
        ++j;
    }

    QString query(QStringLiteral("DELETE FROM ") + tableName);

    if (!where.isEmpty())
        query.append(QStringLiteral(" WHERE ") + whereStr);

    return queryExec(query) ? numRowsAffected() : 0;
}

int Database::update(const QString &tableName, const QVariantMap &updateData, const QVariantMap &where, const QString &whereOperator, const QString &whereComparator)
{
    if (tableName.isEmpty()) {
        emit logMessage(QStringLiteral("Fatal error on try update: The table name is empty!"));
        return 0;
    } else if (where.empty()) {
        emit logMessage(QStringLiteral("Fatal error on try update: The WHERE parameter must be set to update operation!"));
        return 0;
    }

    QString whereStr;
    QString updateValues;
    QVariantList values;

    {
        int k = 0;
        QString key;
        QString separator;
        QMap<QString, QVariant>::const_iterator i = updateData.constBegin();
        while (i != updateData.constEnd()) {
            key = i.key();
            separator = (k++ == 0) ? "" : ",";
            updateValues.append(QString("%1%2=%3").arg(separator, key, ":"+key));
            values << i.value();
            ++i;
        }

        k = 0;
        separator = "";
        i = where.constBegin();
        while (i != where.constEnd()) {
            key = i.key();
            separator = (k++ == 0) ? "" : (QStringLiteral(" ") + whereOperator + QStringLiteral(" "));
            whereStr += QString("%1%2%3%4").arg(separator, key, whereComparator, ":"+key);
            values << i.value();
            ++i;
        }
    }

    openConnection();

    m_qsqlQuery.prepare("UPDATE " + tableName + " SET " + updateValues + " WHERE " + whereStr +";");

    foreach (const QVariant &value, values)
        m_qsqlQuery.addBindValue(value);

    if (m_qsqlQuery.exec())
        return numRowsAffected();

    QString error(lastError());
    if (!error.isEmpty()) {
        error.prepend(QStringLiteral("Fatal error on try update: "));
        emit logMessage(error);
    }
    return 0;
}

int Database::lastInsertId() const
{
    QVariant idTemp = m_qsqlQuery.lastInsertId();
    return idTemp.isValid() ? idTemp.toInt() : 0;
}

int Database::lastRowId(const QString &tableName)
{
    int rowId = 0;
    if (tableName.isEmpty()) {
        emit logMessage(QStringLiteral("Fatal error on lastRowId! The table name is empty!"));
    } else if (queryExec(QString("SELECT ROWID from %1 order by ROWID DESC limit 1").arg(tableName))) {
        while (m_qsqlQuery.next())
            rowId = m_qsqlQuery.value(0).toInt();
    }
    return rowId;
}

int Database::numRowsAffected() const
{
    return m_qsqlQuery.numRowsAffected();
}

QString Database::lastQuery() const
{
    return m_qsqlQuery.lastQuery();
}

QString Database::lastError() const
{
    return m_qsqlQuery.lastError().text();
}
