#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QtWidgets>

class Dialog : public QDialog
{
    Q_OBJECT

public slots:
    void setGrade(const QString &text);
    void setSubject(const QString &text);
private:
    QGridLayout *gLayout;
    QLabel *labelSubject, *labelGrade;
    QComboBox *comboBoxSubject, *comboBoxGrade;
    QPushButton *ok, *cancel;
    QString m_subject, m_grade;
public:
    explicit Dialog(const QStringList &m_listSubject = QStringList {}, QDialog *parent = nullptr);
    const QString& grade() const;
    const QString& subject() const;
    void setSubjectList(const QStringList &subjectList);
signals:
    void gradeChanged(const QString &text);
    void subjectChanged(const QString &text);
};

class AdminDialog: public QDialog
{
    Q_OBJECT
    void fillWindow();
public:
    AdminDialog(QDialog *parent = nullptr);
    const QString username() const;
    const QString password() const;
    const QString rePassword() const;
    bool isAcceptable();
public slots:
    void clearWindow();
private:
    QString m_user, m_pass;
    QGridLayout *gLayout;
    QPushButton *ok, *clear;
    QLineEdit *m_username, *m_password, *m_confirmPassword;
};

class ResultDialog: public QDialog
{
    Q_OBJECT
    void populateComboBoxes();
    struct Courses
    {
        QString courseCode, courseTitle, courseUnit;
        Courses(const QString &code, const QString &title, const QString &unit):
            courseCode(code), courseTitle(title), courseUnit(unit)
        {
            //
        }
    };
private:
    QList<Courses> allCourses;
    QStringList m_courses;
    QLabel *courseCode, *courseTitle, *courseUnit, *score;
    QLineEdit *lineCourseTitle, *lineCourseUnit, *lineCourseScore;
    QComboBox *comboCourseCode;
    QGridLayout *gLayout;
    QPushButton *pushAdd, *pushCancel;
public:
    explicit ResultDialog(const QStringList &courses = {}, QWidget *parent = nullptr);
    void fillGrid();
    const QString getCourseCode() const;
    const QString getCourseUnit() const;
    const QString getCourseTitle() const;
    const QString getCourseScore() const;
signals:

public slots:
    void fillLines(const QString &text);
};

#endif // DIALOG_H
