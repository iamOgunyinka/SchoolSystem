#include "showstudent.h"
#include "ui_showstudent.h"

ShowStudent::ShowStudent(const QString &_matricnumber, const QString &filename, QWidget *parent) :
    QDialog(parent), ui(new Ui::ShowStudent), matric_number(_matricnumber), m_filename(filename)
{
    if(m_filename.isEmpty()){
        QMessageBox::information(this, "FileName", "We realized you didn't continue your registration immediately.\n"
                                 "We need you to locate where your previous file was saved.", QMessageBox::Ok);
        getFileName();
    }
    setWindowTitle(tr("Continue Registration %1").arg(matric_number));
    QString info = QString("Welcome %1!\nClick on \"Add More\" to add your courses and\n%2")
            .arg(_matricnumber)
            .arg("\"Remove\" to remove an existing course chosen by mistake\n");
    QMessageBox::information(this, "Help", info, QMessageBox::Ok);

    ui->setupUi(this);
    setFixedSize(500, 600);
    populateYear();
    enableButtons();

    QObject::connect(ui->pushButtonAddMore, SIGNAL(clicked()), this, SLOT(addSubject()));
    QObject::connect(ui->pushButtonRemove, SIGNAL(clicked()), this, SLOT(removeSubject()));
    QObject::connect(ui->pushButtonAddMore2, SIGNAL(clicked()), this, SLOT(addSubject2()));
    QObject::connect(ui->pushButtonRemove2, SIGNAL(clicked()), this, SLOT(removeSubject2()));

    QObject::connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    setStatusTip("Done");
}

ShowStudent::~ShowStudent()
{
    delete ui;
}

void ShowStudent::getFileName()
{
    m_filename = QFileDialog::getOpenFileName(this, "Open File", QDir::currentPath(), tr("HTML File (*.*)"));
    if(m_filename.isEmpty())
        getFileName();
}

void ShowStudent::accept()
{
    if(ui->listSubject->count() < 4 || ui->listSubject2->count() < 4){
        QMessageBox::critical(this, "Subjects", "Courses chosen are lesser than 4, choose 4 or more",
                              QMessageBox::Ok);
        return;
    } else {
        QString firstSemester("\"" + matric_number + "\", \"" + ui->comboBoxYear->currentText() + "\", "),
                secondSemester("\"" + matric_number + "\", \"" + ui->comboBoxYear->currentText() + "\", ");
        this->setVisible(false);

        //We need 14 items altogether in the database, 2 would be automatically added(matric, year)
        if(ui->listSubject->count() < 12){
            while(ui->listSubject->count() < 12){
                ui->listSubject->addItem(QString("\"\", "));
            }
        }
        if(ui->listSubject2->count() < 12) {
            while(ui->listSubject2->count() < 12){
                ui->listSubject2->setVisible(false);
                ui->listSubject2->addItem(QString("\"\", "));
            }
        }

        for(int i = 0; i != ui->listSubject->count(); ++i){
            if(ui->listSubject->item(i)->text() == QString("\"\", "))
                firstSemester += ui->listSubject->item(i)->text();
            else
                firstSemester += "\"" + ui->listSubject->item(i)->text().replace('\t', ' ') + "\", ";
        }
        for(int j = 0; j != ui->listSubject2->count(); ++j){
            if(ui->listSubject2->item(j)->text() == QString("\"\", "))
                secondSemester += ui->listSubject2->item(j)->text();
            else
                secondSemester += "\"" + ui->listSubject2->item(j)->text().replace('\t', ' ') + "\", ";
        }
        firstSemester.remove(firstSemester.lastIndexOf(", "), 1);
        secondSemester.remove(secondSemester.lastIndexOf(", "), 1);

        DatabaseHandler database;
        if(database){
            if(!database.addNewCourse(firstSemester, secondSemester)){
                QMessageBox::critical(this, "Error", "There was an error trying to save the information",
                                      QMessageBox::Ok);
                return;
            } else {
                printRecord();
                QMessageBox::information(this, "Success", tr("Information has been successfully saved\n"
                                         "Printable HTML has been saved: %1").arg(m_filename),
                                         QMessageBox::Ok);
                this->close();
            }
        }
    }
}

void ShowStudent::populateYear()
{
    QStringList yearsList;
    yearsList << "Year 1" << "Year 2" << "Year 3";
    ui->comboBoxYear->addItems(yearsList);
}

void ShowStudent::enableButtons()
{
    ui->lineMatricNumber->setText(matric_number);
    ui->lineMatricNumber->setEnabled(false);
}

void ShowStudent::addSubject()
{
    CourseDialog *dialog = new CourseDialog(matric_number);
    if(dialog->exec() == QDialog::Accepted){
        QString item = QString(dialog->getCourseCode() + "\t" + dialog->getCourseTitle() + "\t" +
                dialog->getCourseUnit());
        ui->listSubject->addItem(item);
    }
}

void ShowStudent::removeSubject()
{
    delete ui->listSubject->currentItem();
}

void ShowStudent::addSubject2()
{
    CourseDialog *dialog = new CourseDialog(matric_number, this);
    if(dialog->exec() == QDialog::Accepted){
        QString item = QString(dialog->getCourseCode() + "\t" + dialog->getCourseTitle() + "\t" +
                dialog->getCourseUnit());
        ui->listSubject2->addItem(item);
    }
}

void ShowStudent::removeSubject2()
{
    delete ui->listSubject2->currentItem();
}
void ShowStudent::generateHTML(const QList<Courses> &lists, QString &htmlString)
{
    foreach(Courses w, lists){
        htmlString += QString("<tr><td>") + QString(w.courseCode) + QString("</td> <td>") +
                QString(w.courseTitle) + QString("</td><td>") +
                QString(w.courseUnit) + QString("</td></tr>");
    }
}

void ShowStudent::parse(QList<Courses> &lists, const QListWidget *subjects)
{
    for(int i = 0; i != subjects->count(); ++i){
        QString courses = subjects->item(i)->text();
        QStringList splittedCourses = courses.split("\t");
        if(splittedCourses.count() > 2)
            lists << Courses(std::move(splittedCourses[0]),
                             std::move(splittedCourses[1]),
                             std::move(splittedCourses[2]));
    }
}

void ShowStudent::printRecord()
{
    QList<Courses> firstSemester, secondSemester;
    parse(firstSemester, ui->listSubject);
    parse(secondSemester, ui->listSubject2);

    QString resultCode, resultCode_2;
    generateHTML(firstSemester, resultCode);
    generateHTML(secondSemester, resultCode_2);

    QFile iFile(m_filename), oFile(m_filename + "_");
    if(iFile.open(QIODevice::ReadOnly)){
        QTextStream cin(&iFile);
        QString lines;
        while(!(cin.atEnd())){
            lines = cin.readAll();
            lines.replace("__code__", resultCode);
            lines.replace("__code2__", resultCode_2);
        }
        iFile.close();
        if(oFile.open(QIODevice::WriteOnly)){
            QTextStream cout(&oFile);
            cout << lines;
        }
        oFile.close();
        system(QString("rm %1").arg(m_filename).toStdString().c_str());
    } else {
        qDebug() << "Unable to open specified file";
        return;
    }
}

void Courses::readCourseFile(const QString &department, QList<Courses> &courseList)
{
    QFile file(department);
    if(file.open(QIODevice::ReadOnly))
    {
        QTextStream cout(&file);
        QString lines, courseCode, courseUnit;
        while(!cout.atEnd()){
            cout >> courseCode;
            lines = cout.readLine();
            courseUnit = lines.right(1);
            lines.remove(lines.size() - 1, 1);
            courseList.push_back(Courses{std::move(courseCode), std::move(lines), std::move(courseUnit)});
        }
    } else {
        return;
    }
    file.close();
}
CourseDialog::CourseDialog(const QString &m_matric, QWidget *parent):
    QDialog(parent), labelCourseCode(new QLabel(tr("Course code"), this)),
    labelCourseTitle(new QLabel("Course title", this)),
    labelCourseUnit(new QLabel("Course unit", this)),
    comboCourseCode(new QComboBox(this)), lineCourseTitle(new QLineEdit(this)),
    lineCourseUnit(new QLineEdit(this)), ok(new QPushButton("Ok", this)),
    cancel(new QPushButton("Cancel", this)), courseDialogLayout(new QGridLayout(this))
{
    //setAttribute(Qt::WA_DeleteOnClose);
    matric = m_matric;
    populateComboBoxes();
    populateWindowItems();

    QObject::connect(comboCourseCode, SIGNAL(activated(QString)), this, SLOT(fillLineEdits(QString)));
    QObject::connect(ok, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::connect(cancel, SIGNAL(clicked()), this, SLOT(close()));
    QObject::connect(lineCourseTitle, SIGNAL(textChanged(QString)), this, SLOT(setCourseTitle(QString)));
    QObject::connect(lineCourseUnit, SIGNAL(textChanged(QString)), this, SLOT(setCourseUnit(QString)));
    QObject::connect(comboCourseCode, SIGNAL(activated(QString)), this, SLOT(setCourseCode(QString)));
}

void CourseDialog::populateComboBoxes()
{
    DatabaseHandler database;
    if(database){
        database.getDepartment(matric, department);
    }

    allCourseList.clear();
    department.trimmed();
    if(department == tr("Computer Science"))
    {
        Courses().readCourseFile(":/courses/comsci", allCourseList);
    } else if(department == "SLT") {
        Courses().readCourseFile(":/courses/sltdep", allCourseList);
    } else if(department == "Statistics") {
        Courses().readCourseFile(":/courses/stadep", allCourseList);
    } else {
        Courses().readCourseFile(":/courses/fstdep", allCourseList);
    }
    comboCourseCode->clear();
    foreach(Courses w, allCourseList){
        comboCourseCode->addItem(w.courseCode);
    }
}

void CourseDialog::populateWindowItems()
{
    courseDialogLayout->addWidget(labelCourseCode, 0, 0);
    courseDialogLayout->addWidget(comboCourseCode, 0, 1);
    courseDialogLayout->addWidget(labelCourseTitle, 1, 0);
    courseDialogLayout->addWidget(lineCourseTitle, 1, 1);
    courseDialogLayout->addWidget(labelCourseUnit, 2, 0);
    courseDialogLayout->addWidget(lineCourseUnit, 2, 1);
    courseDialogLayout->addWidget(ok, 3, 0);
    courseDialogLayout->addWidget(cancel, 3, 1);
}

void CourseDialog::fillLineEdits(const QString &course_code)
{
    foreach(const Courses i, allCourseList){
        if(i.courseCode == course_code){
            QString m_course;
            m_course = QString("%1\t%2\t%3").arg(i.courseCode)
                    .arg(i.courseTitle).arg(i.courseUnit);
            lineCourseTitle->setText(i.courseTitle.trimmed());
            lineCourseUnit->setText(i.courseUnit);
            break;
        }
    }
}

const QString CourseDialog::getCourseCode() const
{
    return m_courseCode;
}

const QString CourseDialog::getCourseTitle() const
{
    return m_courseTitle;
}

const QString CourseDialog::getCourseUnit() const
{
    return m_courseUnit;
}
void CourseDialog::setCourseCode(const QString &code)
{
    if(code == m_courseCode)
        return;

    m_courseCode = code;
}

void CourseDialog::setCourseTitle(const QString &title)
{
    if(title == m_courseTitle)
        return;
    m_courseTitle = title;
}

void CourseDialog::setCourseUnit(const QString &unit)
{
    if(unit == m_courseUnit)
        return;
    m_courseUnit = unit;
}
