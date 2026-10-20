#include "AuthManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QCryptographicHash>
#include <QCoreApplication>
#include <QDir>
#include <QDebug>

AuthManager::AuthManager() {
}

AuthManager::~AuthManager() {
    if (m_db.isOpen()) {
        m_db.close();
    }
}

QString AuthManager::hashPassword(const QString& password) {
    // Computes cryptographic SHA-256 digest of UTF-8 encoded password
    QByteArray hash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
    return QString(hash.toHex());
}

bool AuthManager::initialize(QString& errorMsg) {
    // Determine database path in the application directory
    QString dbPath = QCoreApplication::applicationDirPath() + "/users.db";

    // Setup SQLite connection
    if (QSqlDatabase::contains("qt_auth_connection")) {
        m_db = QSqlDatabase::database("qt_auth_connection");
    } else {
        m_db = QSqlDatabase::addDatabase("QSQLITE", "qt_auth_connection");
        m_db.setDatabaseName(dbPath);
    }

    if (!m_db.open()) {
        errorMsg = "Failed to open local user database: " + m_db.lastError().text();
        return false;
    }

    // Create the users table if it does not already exist
    QSqlQuery query(m_db);
    QString createTableSql =
        "CREATE TABLE IF NOT EXISTS users ("
        "  username TEXT PRIMARY KEY NOT NULL,"
        "  password_hash TEXT NOT NULL"
        ");";

    if (!query.exec(createTableSql)) {
        errorMsg = "Failed to initialize users table: " + query.lastError().text();
        return false;
    }

    return true;
}

bool AuthManager::registerUser(const QString& username, const QString& password, QString& errorMsg) {
    QString trimmedUser = username.trimmed();
    if (trimmedUser.isEmpty() || password.isEmpty()) {
        errorMsg = "Username and password cannot be empty.";
        return false;
    }

    if (password.length() < 4) {
        errorMsg = "Password must be at least 4 characters long.";
        return false;
    }

    // Check if username already exists
    QSqlQuery checkQuery(m_db);
    checkQuery.prepare("SELECT username FROM users WHERE username = :username;");
    checkQuery.bindValue(":username", trimmedUser);
    if (!checkQuery.exec()) {
        errorMsg = "Database error: " + checkQuery.lastError().text();
        return false;
    }

    if (checkQuery.next()) {
        errorMsg = "Username already exists. Please sign in or choose another.";
        return false;
    }

    // Hash the password and insert into SQLite
    QString hashed = hashPassword(password);
    QSqlQuery insertQuery(m_db);
    insertQuery.prepare("INSERT INTO users (username, password_hash) VALUES (:username, :hash);");
    insertQuery.bindValue(":username", trimmedUser);
    insertQuery.bindValue(":hash", hashed);

    if (!insertQuery.exec()) {
        errorMsg = "Failed to create account: " + insertQuery.lastError().text();
        return false;
    }

    return true;
}

bool AuthManager::authenticateUser(const QString& username, const QString& password, QString& errorMsg) {
    QString trimmedUser = username.trimmed();
    if (trimmedUser.isEmpty() || password.isEmpty()) {
        errorMsg = "Please enter both username and password.";
        return false;
    }

    QString hashed = hashPassword(password);

    QSqlQuery query(m_db);
    query.prepare("SELECT password_hash FROM users WHERE username = :username;");
    query.bindValue(":username", trimmedUser);

    if (!query.exec()) {
        errorMsg = "Database error: " + query.lastError().text();
        return false;
    }

    if (!query.next()) {
        errorMsg = "Username not found. Please register first.";
        return false;
    }

    QString storedHash = query.value(0).toString();
    if (storedHash != hashed) {
        errorMsg = "Incorrect password. Please try again.";
        return false;
    }

    return true;
}
