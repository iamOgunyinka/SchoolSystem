#ifndef ADMINFORM_H
#define ADMINFORM_H

#include <QDialog>
#include <QtWidgets>
#include <QPrintDialog>
#include <QPrinter>
#include "form.h"
#include "existingstudent.h"
#include "studentresult.h"

class AdminForm : public QDialog
{
    Q_OBJECT

    QVBoxLayout *vLayout;

    QMdiArea *childWorkSpace;
    QPushButton *logOut, *addNewStudent, *addNewAdmin, *editStudentRecord,
                *deleteStudentRecord, *addResult, *generateReport;
    QString filename;

    inline void getRecordsFromDB(const QString &department);
    inline void printRecordListPDF(const QStringList &records);
    //void printTable(QPrinter *printer);
    void printTable(QTableView *pTableView, QSqlQueryModel *model,
                    const int &width, const int &height, const int &column, const int &row,
                    const QStringList &lists);
    void parseHTML(const QStringList &lists, QString &htmlString);
public:
    explicit AdminForm(QMdiArea *workSpace = nullptr, QWidget *parent = nullptr);
signals:
    
public slots:
    void addNewStudentForm();
    void addNewResult();
    void newAdmin();
    void reportGenerator();
    void deleteStudent();
protected:
};

#endif // ADMINFORM_H
