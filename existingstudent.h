#ifndef EXISTINGSTUDENT_H
#define EXISTINGSTUDENT_H

#include <QWidget>
#include <QtWidgets>
#include "databasehandler.h"

class ExistingStudent : public QWidget
{
    Q_OBJECT

    QString m_studentRecord, m_matricNumber;
    QPair<bool, QString> isExistsInDatabase(const QString &matric_number);
public:
    explicit ExistingStudent(QWidget *parent = 0);
    QPair<bool, QString> getStudentRecord();
    const QString &studentRecord() const;
    const QString &matricNumber() const;
    void deleteStudent(const QString &matricNumber);
signals:
    
public slots:
    
};

#endif // EXISTINGSTUDENT_H
