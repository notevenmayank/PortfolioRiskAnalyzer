#ifndef AUTHMANAGER_H
#define AUTHMANAGER_H

#include <QString>
#include <QSqlDatabase>

/**
 * @brief The AuthManager class handles local user authentication.
 *
 * It uses a local SQLite database ("users.db") and SHA-256 password hashing.
 * Demonstrates:
 * - Encapsulation of database queries and security logic
 * - Cryptographic hashing via QCryptographicHash
 * - Persistence without requiring any external server or cloud
 */
class AuthManager {
public:
    AuthManager();
    ~AuthManager();

    // Initializes the SQLite database and creates the users table if needed
    bool initialize(QString& errorMsg);

    // Registers a new user with a hashed password
    bool registerUser(const QString& username, const QString& password, QString& errorMsg);

    // Verifies credentials against stored SHA-256 hash
    bool authenticateUser(const QString& username, const QString& password, QString& errorMsg);

    // Utility: Hashes a plain password using SHA-256
    static QString hashPassword(const QString& password);

private:
    QSqlDatabase m_db;
};

#endif // AUTHMANAGER_H
