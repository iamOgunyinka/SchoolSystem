#include "studentresult.h"
#include "ui_studentresult.h"

StudentResult::StudentResult(const QStringList &record, QDialog *parent) :
    QDialog(parent),
    ui(new Ui::StudentResult), localRecord(record)
{
    setFixedSize(480, 400);
    ui->setupUi(this);

    populateComboBoxes();

    ui->lineEditMatric->setText(localRecord[0]);
    ui->lineEditSurname->setText(localRecord[1]);
    ui->lineEditName->setText(localRecord[2]);

    QObject::connect(ui->pushButtonAdd, SIGNAL(clicked()), this, SLOT(pullUpCourses()));
    QObject::connect(ui->pushButtonCancel, SIGNAL(clicked()), this, SLOT(close()));
    QObject::connect(ui->pushButtonRemove, SIGNAL(clicked()), this, SLOT(removeCourse()));
}

StudentResult::~StudentResult()
{
    delete ui;
}

void StudentResult::removeCourse()
{
    delete ui->listWidget->currentItem();
}

void StudentResult::populateComboBoxes()
{
    QStringList years, semester;

    years << "1st year" << "2nd Year" << "3rd Year";
    semester << "1st" << "2nd";

    ui->comboYear->addItems(years);
    ui->comboSemester->addItems(semester);
}

void StudentResult::pullUpCourses()
{
    DatabaseHandler db;
    if(db){
        QStringList semester;
        if(ui->comboSemester->currentText() == "1st"){
            db.getSemesterCourses(localRecord[0], "1st", semester);
        } else {
            db.getSemesterCourses(localRecord[0], "2nd", semester);
        }
        if(semester.count() != 0){
            openDialog(semester);
        } else {
            QMessageBox::critical(this, "Error", "An error occurred", QMessageBox::Ok);
            this->close();
        }
    }
}

void StudentResult::openDialog(const QStringList &courses)
{
    ResultDialog resultDialog(courses, this);
    if(resultDialog.exec() == QDialog::Accepted){
        QString item = QString(resultDialog.getCourseCode() + "\t" + resultDialog.getCourseTitle() +
                               "\t" + resultDialog.getCourseUnit() + "\t" + resultDialog.getCourseScore());

        ui->listWidget->addItem(item);
    }
}
