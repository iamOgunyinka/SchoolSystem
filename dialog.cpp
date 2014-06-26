#include "dialog.h"

Dialog::Dialog(const QStringList &m_listSubject, QDialog *parent): QDialog(parent),
    gLayout(new QGridLayout), labelSubject(new QLabel("Subject")), labelGrade(new QLabel("Grade")),
    comboBoxSubject(new QComboBox), comboBoxGrade(new QComboBox),
    ok(new QPushButton("Accept")), cancel(new QPushButton("Cancel"))
{
    //setAttribute(Qt::WA_DeleteOnClose);

    QStringList listSubject, listGrade;
    if(m_listSubject.size() == 0){
        listSubject << "English Language" << "Mathematics" << "Biology" << "Agricultural Science"
                    << "Physics" << "Chemistry" << "Economics" << "Yoruba" << "Geography"
                    << "Government" << "Account" << "Literature in English" << "Commerce"
                    << "Civic Education" << "CRK" << "CRS" << "Further Maths" << "Igbo"
                    << "Hausa";
    } else {
        listSubject.append(m_listSubject);
    }
    listGrade << "A1" << "B2" << "B3" << "C4" << "C5" << "C6" << "D7" << "E8" << "F9";
    listSubject.sort(Qt::CaseInsensitive);

    comboBoxSubject->addItems(listSubject);
    comboBoxGrade->addItems(listGrade);

    gLayout->addWidget(labelSubject, 0, 0);
    gLayout->addWidget(labelGrade, 1, 0);
    gLayout->addWidget(comboBoxSubject, 0, 1);
    gLayout->addWidget(comboBoxGrade, 1, 1);
    gLayout->addWidget(ok, 2, 1);
    gLayout->addWidget(cancel, 2, 2);

    this->setLayout(gLayout);
    QObject::connect(cancel, SIGNAL(clicked()), this, SLOT(close()));
    QObject::connect(ok, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::connect(comboBoxSubject, SIGNAL(activated(QString)), this, SLOT(setSubject(QString)));
    QObject::connect(comboBoxGrade, SIGNAL(activated(QString)), this, SLOT(setGrade(QString)));
}

const QString& Dialog::subject() const
{
    return m_subject;
}

const QString& Dialog::grade() const
{
    return m_grade;
}

void Dialog::setGrade(const QString &text)
{
    if(text == m_grade || text == "")
        return;
    m_grade = text;
    emit gradeChanged(text);
}

void Dialog::setSubject(const QString &text)
{
    if(text == m_subject || text == "")
        return;
    m_subject = text;
    emit subjectChanged(text);
}

void Dialog::setSubjectList(const QStringList &subjectList)
{
    comboBoxSubject->clear();
    comboBoxSubject->addItems(subjectList);
}

AdminDialog::AdminDialog(QDialog *parent): QDialog(parent),
    gLayout(new QGridLayout(this)), ok(new QPushButton("Add", this)),
    clear(new QPushButton("Clear", this)),
    m_username(new QLineEdit), m_password(new QLineEdit), m_confirmPassword(new QLineEdit)
{
    fillWindow();

    this->setLayout(gLayout);
    QObject::connect(ok, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::connect(clear, SIGNAL(clicked()), this, SLOT(clearWindow()));
}

void AdminDialog::fillWindow()
{
    gLayout->addWidget(new QLabel("Username", this), 0, 0);
    gLayout->addWidget(m_username, 0, 1);
    gLayout->addWidget(new QLabel("Password", this), 1, 0);
    gLayout->addWidget(m_password, 1, 1);
    gLayout->addWidget(new QLabel("Confirm Password", this), 2, 0);
    gLayout->addWidget(m_confirmPassword, 2, 1);
    gLayout->addWidget(ok, 3, 1);
    gLayout->addWidget(clear, 3, 2);

    m_password->setEchoMode(QLineEdit::Password);
    m_confirmPassword->setEchoMode(QLineEdit::Password);
}

const QString AdminDialog::username() const
{
    return m_username->text().trimmed();
}

const QString AdminDialog::password() const
{
    return m_password->text();
}

const QString AdminDialog::rePassword() const
{
    return m_confirmPassword->text();
}
void AdminDialog::clearWindow()
{
    m_username->clear();
    m_password->clear();
    m_confirmPassword->clear();
    m_username->setFocus();
}

bool AdminDialog::isAcceptable()
{
    if(password().size() > 7 && rePassword() == password())
        return true;
    return false;
}

ResultDialog::ResultDialog(const QStringList &courses, QWidget *parent):
    QDialog(parent), m_courses(courses), courseCode(new QLabel("Course Code", this)),
    courseTitle(new QLabel("Course Title", this)), courseUnit(new QLabel("Course Unit", this)),
    score(new QLabel("Score in Alphabet", this)), lineCourseTitle(new QLineEdit(this)),
    lineCourseUnit(new QLineEdit(this)), lineCourseScore(new QLineEdit(this)),
    comboCourseCode(new QComboBox(this)), gLayout(new QGridLayout(this)),
    pushAdd(new QPushButton("Add", this)), pushCancel(new QPushButton("Cancel", this))
{
    populateComboBoxes();
    fillGrid();
    QObject::connect(comboCourseCode, SIGNAL(activated(QString)), this, SLOT(fillLines(const QString &)));
    QObject::connect(pushCancel, SIGNAL(clicked()), this, SLOT(close()));
    QObject::connect(pushAdd, SIGNAL(clicked()), this, SLOT(accept()));
}

void ResultDialog::populateComboBoxes()
{
    for(QStringList::iterator i = m_courses.begin(); i != m_courses.end(); ++i)
    {
        QString code = i->left(i->indexOf(' ')), title, unit = i->at(i->size() - 1);
        title = i->remove(code, Qt::CaseInsensitive).remove(i->right(2));
        allCourses.append(Courses{code, title, unit});
    }
    foreach(ResultDialog::Courses w, allCourses)
        comboCourseCode->addItem(w.courseCode);
}

void ResultDialog::fillGrid()
{
    gLayout->addWidget(courseCode, 0, 0);
    gLayout->addWidget(comboCourseCode, 0, 1);
    gLayout->addWidget(courseTitle, 1, 0);
    gLayout->addWidget(lineCourseTitle, 1, 1);
    gLayout->addWidget(courseUnit, 2, 0);
    gLayout->addWidget(lineCourseUnit, 2, 1);
    gLayout->addWidget(score, 3, 0);
    gLayout->addWidget(lineCourseScore, 3, 1);
    gLayout->addWidget(pushAdd, 4, 0);
    gLayout->addWidget(pushCancel, 4, 1);
}

void ResultDialog::fillLines(const QString &text)
{
    foreach(const Courses c, allCourses){
        if(c.courseCode == text){
            lineCourseTitle->setText(c.courseTitle);
            lineCourseUnit->setText(c.courseUnit);
        }
    }
}

const QString ResultDialog::getCourseCode() const
{
    return comboCourseCode->currentText();
}
const QString ResultDialog::getCourseTitle() const
{
    return lineCourseTitle->text();
}

const QString ResultDialog::getCourseUnit() const
{
    return lineCourseUnit->text();
}

const QString ResultDialog::getCourseScore() const
{
    return lineCourseScore->text();
}
