#include "adminform.h"

AdminForm::AdminForm(QMdiArea *workSpace, QWidget *parent) :
    QDialog(parent), vLayout(new QVBoxLayout), childWorkSpace(workSpace),
    logOut(new QPushButton("&Log Out", this)), addNewStudent(new QPushButton("New Student", this)),
    addNewAdmin(new QPushButton("&Add New Admin", this)),
    editStudentRecord(new QPushButton("Edit Student Record", this)),
    deleteStudentRecord(new QPushButton("&Delete Student", this)),
    addResult(new QPushButton("Add New Result")),
    generateReport(new QPushButton("Generate Report", this)),
    filename("")
{
    QGroupBox *groupBox = new QGroupBox(this);
    vLayout->addWidget(logOut);
    vLayout->addWidget(addNewStudent);
    vLayout->addWidget(addNewAdmin);
    vLayout->addWidget(generateReport);
    vLayout->addWidget(addResult);
    vLayout->addWidget(editStudentRecord);
    vLayout->addWidget(deleteStudentRecord);

    groupBox->setLayout(vLayout);
    setLayout(vLayout);

    QObject::connect(logOut, SIGNAL(clicked()), this, SLOT(close()));
    QObject::connect(addNewStudent, SIGNAL(clicked()), this, SLOT(addNewStudentForm()));
    QObject::connect(addNewAdmin, SIGNAL(clicked()), this, SLOT(newAdmin()));
    QObject::connect(addResult, SIGNAL(clicked()), this, SLOT(addNewResult()));
    QObject::connect(generateReport, SIGNAL(clicked()), this, SLOT(reportGenerator()));
    QObject::connect(deleteStudentRecord, SIGNAL(clicked()), this, SLOT(deleteStudent()));
}

void AdminForm::addNewStudentForm()
{

    Form *addForm(new Form(filename));
    addForm->setWindowModality(Qt::WindowModality::ApplicationModal);
    if(childWorkSpace)
        childWorkSpace->addSubWindow(addForm);

    addForm->show();
}

void AdminForm::newAdmin()
{
    AdminDialog login;
    DatabaseHandler db;
    if(login.exec() == QDialog::Accepted){
        if(login.isAcceptable() && db){
            try{
                db.addNewAdmin(login.username(), login.password());
            } catch (const QString & errorMsg) {
                QMessageBox::critical(this, "Error", errorMsg, QMessageBox::Ok);
                return;
            }
            QMessageBox::information(nullptr, "Add new", "New administrator added.", QMessageBox::Ok);
        } else {
            QMessageBox::critical(this, "Error", "An unrecoverable error occurred", QMessageBox::Ok);
        }
    }
}

void AdminForm::addNewResult()
{
    ExistingStudent student;
    QString record = student.studentRecord();
    if(record == ""){
        switch(QMessageBox::information(this, "Search", "No such student exists. Try again?",
                                 QMessageBox::Yes | QMessageBox::No))
        {
        case QMessageBox::No:
            return;
        case QMessageBox::Yes:
            addNewResult();
        default:
            return;
        }
    } else {
        QStringList studentsRecord = record.split('\t');

        QMessageBox::information(this, "Found", QString("Welcome %1 %2.")
                                 .arg(studentsRecord[1]).arg(studentsRecord[2]), QMessageBox::Ok);
        StudentResult *resultDialog = new StudentResult(studentsRecord, this);
        childWorkSpace->addSubWindow(resultDialog);
        if(resultDialog->exec() == QDialog::Accepted){
            resultDialog->close();
            childWorkSpace->removeSubWindow(resultDialog);
        }
    }
    return;
}

void AdminForm::reportGenerator()
{
    QStringList dept {"All", "Computer Science", "Food Tech", "SLT", "Statistics" };
    QString department = QInputDialog::getItem(this, "Generate Report",
                                               "Choose department", dept);
    if(department.isNull()){
        return;
    } else {
        getRecordsFromDB(department);
    }
}

void AdminForm::getRecordsFromDB(const QString &department)
{
    QString query(QString("SELECT application_number as \"Application Number\", "
                          "surname as Surname, other_names as \"Other Names\", sex as Sex, department, "
                          "state FROM newStudents %1").arg(department == QString("All") ?
                                    QString(""): QString("WHERE department = \"%1\"")
                                                               .arg(department)));
    QStringList lists;
    DatabaseHandler db;
    if(db){
        try{
            db.getList(query, lists);
            QPair<QTableView *, QSqlQueryModel *> records = db.viewAllRecords(query);
            records.first->setModel(records.second);
            records.first->resizeColumnsToContents();
            int width = 0, height = 0, columns = records.second->columnCount(),
                    rows = records.second->rowCount();
            for(int i = 0; i != columns; ++i){
                width += records.first->columnWidth(i);
            }
            for(int x = 0; x != rows; ++x){
                height += records.first->rowHeight(x);
            }
            records.first->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            //records.first->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            switch(QMessageBox::information(this, this->windowTitle(), "Do you wish to print report?",
                                     QMessageBox::Yes | QMessageBox::No))
            {
            case QMessageBox::Yes:
                printTable(records.first, records.second, width, height, columns, rows, lists);
                break;
            case QMessageBox::No:
                break;
            default:
                break;
            }
        } catch(const QString &error) {
            QMessageBox::critical(this, "", error, QMessageBox::Ok);
        }
    } else {
        QMessageBox::critical(this, "From AdminForm::createDatabaseHandler", "An error occured",
                              QMessageBox::Ok);
        return;
    }
}

void AdminForm::printTable(QTableView *pTableView, QSqlQueryModel *model, const int &width, const int &height, const int &column, const int &row, const QStringList &lists)
{
    pTableView->setFixedSize(width + 20, height + 20);

    QString printModel = QInputDialog::getItem(this, "Choose style", "Application Internal Format Table, HTML or PDF?",
                                                {"Internal", "PDF", "HTML"}, 0, false);
    if(printModel == QString("PDF")){
        pTableView->setModel(model);
        QPrinter printer;
        QPrintDialog printDialog(&printer, this);
        if(printDialog.exec() == QDialog::Accepted){
            printer.setOrientation(QPrinter::Portrait);
            QPoint startPoint = QPoint(20, 20);
            QRegion printRegion = QRegion(0, 0, width + 20, height + 20);
            for(int i = 0; i <= row; ++i){
                pTableView->render(&printer, startPoint, printRegion, QWidget::DrawChildren);
            }
        }
        QMessageBox::information(this, "Report Printed", "Your PDF report has been generated successfully!",
                                 QMessageBox::Ok);

    } else if( printModel == QString("Internal") ) {
        childWorkSpace->addSubWindow(pTableView);
        pTableView->show();

    } else {
        QFile iFile(":/HTML/print.html"), oFile("/home/matrix/Desktop/Report.html");
        QString htmlString;
        if(iFile.open(QIODevice::ReadOnly) && oFile.open(QIODevice::WriteOnly)){
            parseHTML(lists, htmlString);
            QTextStream cin(&iFile), cout(&oFile);
            QString allFileContents;
            while(!cin.atEnd()){
                allFileContents = cin.readAll();
                allFileContents.replace("__code__", htmlString);
            }
            cout << allFileContents;
            iFile.close(); oFile.close();
        } else {
            QMessageBox::critical(this, "Error", "", QMessageBox::Ok);
            return;
        }
        QMessageBox::information(this, "Success", "Report has been successfully generated to the Desktop"
                                 ".\nUnder the name \"Report.html\".", QMessageBox::Ok);
    }
}

void AdminForm::parseHTML(const QStringList &lists, QString &htmlString)
{
    int sn = 1;
    foreach(QString str, lists){
        QStringList splitted = str.split("\t");
        htmlString += "<tr>" + QString("<td> %1 </td>").arg(sn);
        foreach (QString s, splitted) {
            htmlString += "<td>" + s.toUpper() + "</td> ";
        }
        htmlString += "</tr>\n";
        ++sn;
    }
    qDebug() << htmlString;
}

void AdminForm::deleteStudent()
{
    ExistingStudent student;
    QString matricString = student.matricNumber();
    if(student.studentRecord() != QString("")){
        switch(QMessageBox::question(this, windowTitle(), "Are you sure you wanna delete record?",
                                     QMessageBox::Yes | QMessageBox::No, QMessageBox::No))
        {
        case QMessageBox::Yes:
            try {
                student.deleteStudent(matricString);
            } catch (const QString &e) {
                QMessageBox::critical(this, windowTitle(), e, QMessageBox::Ok);
                return;
            }
            QMessageBox::information(this, "Delete Student", QString("All information related to"
                                                                 "%1 has been deleted").arg(matricString),
                                 QMessageBox::Ok);
            break;

        default:
            QMessageBox::information(this, "Deletion", "Deletion has been cancelled", QMessageBox::Ok);
            break;
        }
    } else {
        switch(QMessageBox::information(this, "Deletion", "No such Student Exists. Try again?",
                                        QMessageBox::Yes | QMessageBox::No, QMessageBox::No)){
        case QMessageBox::Yes:
            deleteStudent();
            break;
        default:
            break;
        }
    }
}
