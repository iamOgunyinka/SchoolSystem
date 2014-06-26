#include "existingstudent.h"

ExistingStudent::ExistingStudent(QWidget *parent) :
    QWidget(parent)
{
    m_studentRecord = getStudentRecord().second;
}

QPair<bool, QString> ExistingStudent::getStudentRecord()
{
    bool ok = false;
    QString matric = QInputDialog::getText(nullptr, tr(""), tr("Input matric number")
                                           , QLineEdit::Normal, "", &ok);
    if(ok && !matric.isEmpty()){
        m_matricNumber = matric;
        QPair<bool, QString> isExists = isExistsInDatabase(matric);

        if(isExists.first){
            return isExists;
        } else {
            return {false, QString{""}};
        }
    }
    return {false, QString("")};
}

QPair<bool, QString> ExistingStudent::isExistsInDatabase(const QString &matric_number)
{
    DatabaseHandler database;
    QString text;
    if(database){
        if((text = database.findMatricNumber(matric_number)) != QString("")){
            return {true, text};
        } else {
            return {false, QString{}};
        }
    }
    return {false, QString{}};
}

const QString& ExistingStudent::studentRecord() const
{
    return m_studentRecord;
}
const QString &ExistingStudent::matricNumber() const
{
    return m_matricNumber;
}

void ExistingStudent::deleteStudent(const QString &matricNumber)
{
    DatabaseHandler db;
    if(db){
        QString query (QString("DELETE FROM newStudents WHERE newStudents.application_number = \"%1\"")
                       .arg(matricNumber));
        if(!db.execute(query)){
            throw QString("Unable to delete record");
        }
    }
}
