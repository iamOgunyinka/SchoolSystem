#include "databasehandler.h"

DatabaseHandler::DatabaseHandler() noexcept(false):
    DatabaseHandler("admin", "admin")
{
    //Work delegated.
}

DatabaseHandler::DatabaseHandler(const QString &username, const QString &password):
    m_username(username), m_password(password)
{
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setDatabaseName("school");
    db.setHostName("localhost");
    db.setUserName(m_username);
    db.setPassword(m_password);
}

void DatabaseHandler::setUsername(const QString &username)
{
    if(username == m_username)
        return;
    m_username = username;
}

void DatabaseHandler::setPassword(const QString &password)
{
    if(password == m_password)
        return;
    m_password = password;
}

bool DatabaseHandler::login()
{
    if(db.isOpen())
        db.close();

    if(!db.open()){
        return false;
    } else {
        createNewTable();
        if(!createTableSubjects("subjectFirstSitting")){
            return false;
        }
        if(!createTableSubjects("subjectSecondSitting")){
            return false;
        }
        if(!createPasswordTable()){
            return false;
        }
        createAdmins();
        return true;
    }
    return true;
}

QPair<QTableView *, QSqlQueryModel *> DatabaseHandler::viewAllRecords(const QString &query)
{
    QSqlQueryModel *model(new QSqlQueryModel);
    model->setQuery(query);

    sp::unique_ptr<QTableView> view(new QTableView);
    view->setModel( model );
    return {view.release(), model};
}

bool DatabaseHandler::execute(const QString &query)
{
    QSqlQuery qry;
    qry.prepare(query);

    if(!qry.exec()){
        QMessageBox::critical(NULL, "Database Error", qry.lastError().text(), QMessageBox::Ok);
        return false;
    } else {
        return true;
    }
}

bool DatabaseHandler::addNewStudent(const QStringList &entry, const QStringList &subject, const QStringList &subject2)
{
    QString queryString;
    extractSubjects(entry, queryString);

    bool queryAddSecondSubjectsSucceeded = true;
    QString new_string(QString("INSERT INTO newStudents VALUES(%1)").arg(queryString).trimmed());

    queryString.clear();
    extractSubjects(subject, queryString, entry[0]);
    QString newSubjectString(QString("INSERT INTO subjectFirstSitting VALUES(%1)")
                       .arg(queryString).trimmed());

    queryString.clear();
    QString newSubjectString2;
    if(subject2.size() != 0){
        extractSubjects(subject2, queryString, entry[0]);
        newSubjectString2 = QString("INSERT INTO subjectSecondSitting VALUES(%1)")
                           .arg(queryString).trimmed();

        queryAddSecondSubjectsSucceeded = this->execute(newSubjectString2);
    }
    if(!(this->execute(new_string) && this->execute(newSubjectString) && queryAddSecondSubjectsSucceeded))
    {
        db.rollback();
        return false;
    } else {
        return true;
    }
}

bool DatabaseHandler::createNewTable()
{
    QSqlQuery qry;
    qry.prepare("CREATE TABLE IF NOT EXISTS newStudents("
                "application_number varchar(30) PRIMARY KEY, "
                "surname varchar(30), other_names varchar(50), address varchar(100),"
                "sex varchar(7),  level varchar(5),  session varchar(20), "
                "marital_Status varchar(20), religion varchar(20), state varchar(20), "
                "department varchar(20), DOB varchar(10), exam_number_1 varchar(20), "
                "exam_number_2 varchar(20), exam_type_1 varchar(20), exam_type_2 varchar(20), "
                "year varchar(4), year_2 varchar(4))");

    if(!qry.exec()){
        QMessageBox::critical(NULL, "Database Error", qry.lastError().text(), QMessageBox::Ok);
        return false;
    }
    return true;
}

DatabaseHandler::~DatabaseHandler()
{
    db.close();
}

bool DatabaseHandler::createTableSubjects(const QString &tableName)
{
    QSqlQuery qry;
    qry.prepare(QString("CREATE TABLE IF NOT EXISTS %1 ("
                        "application_number varchar(20) PRIMARY KEY, exam_number varchar(20), "
                        "subject1 varchar(50), subject2 varchar(50), subject3 varchar(50), "
                        "subject4 varchar(50), subject5 varchar(50), subject6 varchar(50), "
                        "subject7 varchar(50), subject8 varchar(50), subject9 varchar(50))")
                .arg(tableName));
    if(!qry.exec()){
        QMessageBox::critical(NULL, "Database Error", qry.lastError().text(), QMessageBox::Ok);
        return false;
    } else {
        return true;
    }
}

bool DatabaseHandler::createPasswordTable()
{
    QSqlQuery qry;
    qry.prepare(QString("CREATE TABLE IF NOT EXISTS passwords "
                        "(application_number varchar(20) PRIMARY KEY, "
                        "password varchar(100))"));
    if(!qry.exec()){
        return false;
    } else {
        return true;
    }
}

bool DatabaseHandler::addNewPassword(const QPair<const QString, const QString> &passwordField)
{
    QSqlQuery qry;
    qry.prepare(QString("INSERT INTO passwords VALUES(\"%1\", \"%2\")")
                .arg(passwordField.first)
                .arg(passwordField.second));
    if(!qry.exec()){
        return false;
    } else {
        return true;
    }
}

QString DatabaseHandler::lookupMatricNumber(const QString &text)
{
    QString m_text = lookupString(text);
    return m_text;
}

void DatabaseHandler::extractSubjects(const QStringList &subject, QString &studentList, const QString &entry)
{
    static int i = 0;
    if(subject.size() != 0){
        studentList += (entry.isEmpty() ? "" : ("\"" + entry + "\", "));
        foreach (QString text, subject) {
            studentList += "\"" + text + "\", ";
        }
        if(i > 0 && subject.size() < 11){
            int cols = (11 - subject.size());
            for(int i = 0; i < cols - 1; ++i){
                studentList += "\"\", ";
            }
        }
        studentList.remove(studentList.lastIndexOf(", "), 1);
    }
    ++i;
}

bool DatabaseHandler::checkLoginStatus(const QPair<QString, QString> &pairUsernamePassword)
{
    QSqlQuery qry;
    QString queryString(QString("SELECT password from passwords WHERE application_number = \"%1\"")
                        .arg(pairUsernamePassword.first));
    qry.prepare(queryString);
    if(!qry.exec())
        return false;
    if(qry.record().count() < 1)
        return false;

    while(qry.next()){
        QString password(qry.value(0).toString());
        if(pairUsernamePassword.second == password){
            return true;
        }
    }
    return false;
}

QString DatabaseHandler::lookupString(QString text)
{
    QString temp {};
    if(text.isEmpty())
        text = QInputDialog::getText(nullptr, "", "Enter search string");

    if(!text.isEmpty()){
        QSqlQuery qry;
        qry.prepare("SELECT application_number FROM new_student");
        if(!qry.exec())
            return QString{};

        QSqlRecord record = qry.record();
        int count = record.count();

        while(qry.next()){
            temp = "";
            for(int i = 0; i != count; ++i){
                temp += qry.value(i).toString() + (i < count-1 ? "\t": "");
            }
            if(temp.split('\t').contains(text, Qt::CaseInsensitive))
                return temp;
        }
    }
    return QString{};
}

void DatabaseHandler::getDepartment(const QString &matric, QString &department)
{
    QSqlQuery qry;
    qry.prepare(QString("SELECT department FROM newStudents WHERE application_number = \"%1\"")
                .arg(matric));
    if(!qry.exec()){
        department = QString {};
        return;
    } else {
        while(qry.next())
            department = qry.value(0).toString();
    }
}

bool DatabaseHandler::addNewCourse(const QString &firstSemester, const QString &secondSemester)
{
    QSqlQuery qry;
    qry.prepare(QString("CREATE TABLE IF NOT EXISTS firstSemester("
                        "application_number varchar(20) PRIMARY KEY, level varchar(20), "
                        "course1 varchar(100), course2 varchar(100), course3 varchar(100), "
                        "course4 varchar(100), course5 varchar(100) NULL, course6 varchar(100) NULL, "
                        "course7 varchar(100) NULL, course8 varchar(100) NULL, course9 varchar(100) NULL, "
                        "course10 varchar(100) NULL, course11 varchar(100) NULL, course12 varchar(100) NULL)"));
    if(!qry.exec())
        return false;
    qry.prepare(QString("CREATE TABLE IF NOT EXISTS secondSemester("
                        "application_number varchar(20) PRIMARY KEY, level varchar(20), "
                        "course1 varchar(100), course2 varchar(100), course3 varchar(100), "
                        "course4 varchar(100), course5 varchar(100) NULL, course6 varchar(100) NULL, "
                        "course7 varchar(100) NULL, course8 varchar(100) NULL, course9 varchar(100) NULL, "
                        "course10 varchar(100) NULL, course11 varchar(100) NULL, course12 varchar(100) NULL)"));
    if(!qry.exec())
        return true;

    QString query(QString("INSERT INTO firstSemester VALUES (%1)").arg(firstSemester));
    QString query2(QString("INSERT INTO secondSemester VALUES (%1)").arg(secondSemester));

    if(!qry.exec(query)){
        QMessageBox::critical(nullptr, "ADD NEW COURSE", qry.lastError().text(), QMessageBox::Ok);
        return false;
    }
    qry.prepare(query2);
    if(!qry.exec()){
        QMessageBox::critical(nullptr, "ADD NEW COURSE 2", qry.lastError().text(), QMessageBox::Ok);
        return false;
    }

    return true;
}

void DatabaseHandler::createAdmins()
{
    QSqlQuery qry;
    qry.prepare("CREATE TABLE IF NOT EXISTS adminTable(username varchar(30) PRIMARY KEY, "
                  "password varchar(50))");
    qry.exec();

    qry.prepare(QString("INSERT INTO adminTable VALUES(\"%1\", \"%2\")").arg("admin").arg("admin"));
    qry.exec();
}

bool DatabaseHandler::loginAdmin(const QString &user, const QString &pass)
{
    QSqlQuery qry;
    QString queryText(QString("SELECT password FROM adminTable WHERE username = \"%1\"")
                      .arg(user));
    qry.prepare(queryText);
    if(!qry.exec())
        return false;

    QString returnedPassword {};
    while(qry.next())
        returnedPassword = qry.value(0).toString();
    if(returnedPassword != pass)
        return false;

    return true;
}

void DatabaseHandler::addNewAdmin(const QString &username, const QString &password)
{
    QSqlQuery qry;
    qry.prepare(QString("INSERT INTO adminTable VALUES(\"%1\", \"%2\")")
                .arg(username).arg(password));
    if(!qry.exec())
        throw QString("Unable to add new Admin\nReason: %1").arg(qry.lastError().text());
}

QString DatabaseHandler::findMatricNumber(const QString &matric) const
{
    QSqlQuery qry(QString("SELECT * FROM newStudents WHERE application_number = \"%1\"")
                  .arg(matric));
    if(!qry.exec())
        return QString("");

    QString text;
    while(qry.next()){
        text = "";
        for(int i = 0; i != qry.record().count(); ++i)
            text += qry.value(i).toString() + (i < qry.record().count() - 1 ? "\t": "");
    }
    return text;
}

void DatabaseHandler::getSemesterCourses(const QString &appNumber, const QString &semester, QStringList &firstSemester)
{
    QSqlQuery qry;
    qry.prepare(QString("SELECT * FROM %1 WHERE application_number = \"%2\"")
                .arg(semester == "1st" ? "firstSemester": "secondSemester")
                .arg(appNumber));
    if(!qry.exec())
        return;

    QString concat;
    while(qry.next()){
        concat = "";
        for(int i = 0; i != qry.record().count(); ++i){
            if(qry.value(i).toString() != "")
                concat += qry.value(i).toString() + "\n";// + (i == qry.record().count() -1 ? "\n": "");
        }
        firstSemester.append(concat.split("\n"));
    }
    firstSemester.removeFirst();
    firstSemester.removeFirst();
    firstSemester.removeLast();
}

void DatabaseHandler::getList(const QString &query, QStringList &list) noexcept(false)
{
    QSqlQuery qry(query);
    if(!qry.exec())
        throw qry.lastError().text();

    QString row;
    int cols = qry.record().count();
    while(qry.next()){
        row = "";
        for(int i = 0; i != cols; ++i){
            row += qry.value(i).toString() + ((i < cols-1) ? "\t" : "");
        }
        list.append(row);
    }
}
