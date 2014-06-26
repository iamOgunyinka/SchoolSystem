#ifndef DATABASEHANDLER_H
#define DATABASEHANDLER_H

#include <QtSql>
#include <QtWidgets>
#include <QString>
#include "memory.h"

class DatabaseHandler
{
public:
    DatabaseHandler() noexcept(false);
    DatabaseHandler(const QString &username, const QString &password);

    bool createNewTable();
    bool createTableSubjects(const QString &tableName);
    bool createPasswordTable();

    bool execute(const QString &query);
    bool addNewStudent(const QStringList &entry, const QStringList &subject = QStringList{},
             const QStringList &subject2 = QStringList{});
    bool addNewPassword(const QPair<const QString, const QString> &passwordField);
    QPair<QTableView *, QSqlQueryModel *> viewAllRecords(const QString &query);
    ~DatabaseHandler();

    QString lookupMatricNumber(const QString &text);
    QString findMatricNumber(const QString &matric) const;
    QString lookupString(QString text);
    bool checkLoginStatus(const QPair<QString, QString> &pairUsernamePassword);
    bool login();
    bool loginAdmin(const QString &user, const QString &pass);
    void setUsername(const QString &username);
    void setPassword(const QString &password);
    operator bool(){
        return login();
    }
    void createAdmins();
    bool addNewCourse(const QString &firstSemester, const QString &secondSemester);
    void addNewAdmin(const QString &username, const QString &password);
    void extractSubjects(const QStringList &subject, QString &studentList,
                         const QString &entry = QString{});
    void getDepartment(const QString &matric, QString &department);
    void getSemesterCourses(const QString &appNumber, const QString &semester, QStringList &firstSemester);

    void getList(const QString &query, QStringList &list) noexcept(false);
private:

    QSqlDatabase db;
    QSqlRecord record;
    QString m_username, m_password;
};

#endif // DATABASEHANDLER_H
