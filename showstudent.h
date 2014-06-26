#ifndef SHOWSTUDENT_H
#define SHOWSTUDENT_H

#include <QDialog>
#include <QtWidgets>
#include "databasehandler.h"
#include <vector>
#include <fstream>

class CourseDialog;

namespace Ui {
class ShowStudent;
}

class Courses;
class ShowStudent : public QDialog
{
    Q_OBJECT
    typedef QPair<QString, QString> Pair;
    void parse(QList<Courses> &lists, const QListWidget *subjects);
    void printRecord();
    void generateHTML(const QList<Courses> &lists, QString &htmlString);
public slots:
    void addSubject();
    void removeSubject();
    void addSubject2();
    void removeSubject2();
    void accept();
public:
    friend class CourseDialog;
    explicit ShowStudent(const QString &_matricnumber = QString{}, const QString &filename = QString{}, QWidget *parent = nullptr);
    ~ShowStudent();
    void populateYear();
    void enableButtons();
    void getFileName();
    QPair<bool, QString> getStudentRecord();
    QPair<bool, QString> isExistsInDatabase(const QString &matric_number);
private:
    Ui::ShowStudent *ui;
    const QString matric_number;
    QString m_filename;
};

struct Courses
{
    QString courseCode, courseTitle, courseUnit;
    Courses() = default;
    Courses(const QString &code, const QString &title, const QString &unit):
        courseCode(code), courseTitle(title), courseUnit(unit) {}
    Courses(QString &&code, QString &&title, QString &&unit):
        courseCode(std::move(code)), courseTitle(std::move(title)), courseUnit(std::move(unit))
    { }

    void readCourseFile(const QString &department, QList<Courses> &courseList);
};

class CourseDialog: public QDialog
{
    Q_OBJECT
    void populateComboBoxes();
    void populateWindowItems();
public:
    explicit CourseDialog(const QString &m_matric, QWidget *parent = nullptr);
    const QString getCourseCode() const;
    const QString getCourseTitle() const;
    const QString getCourseUnit() const;

public slots:
    void fillLineEdits(const QString &course_code);
    void setCourseCode(const QString &code);
    void setCourseTitle(const QString &title);
    void setCourseUnit(const QString &unit);
private:
    QLabel *labelCourseCode, *labelCourseTitle, *labelCourseUnit;
    QComboBox *comboCourseCode;
    QLineEdit *lineCourseTitle, *lineCourseUnit;
    QPushButton *ok, *cancel;

    //the "key" is the "courseCode", the "value" is the pair("course title" and "course unit")
    QList<Courses> allCourseList;
    QString m_courseCode, department, matric, m_courseTitle, m_courseUnit;
    QGridLayout *courseDialogLayout;
};

#endif // SHOWSTUDENT_H
