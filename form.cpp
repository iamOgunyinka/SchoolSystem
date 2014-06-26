#include "form.h"
#include "ui_form.h"

Form::Form(QString &filename, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form), dialog(new Dialog), m_filename(filename)
{
    setFixedSize(680, 600);
    ui->setupUi(this);

    enableExams();
    populateComboBoxes();
    populateSpinBoxes();
    QIntValidator *validator = new QIntValidator;
    ui->lineExamNumber->setValidator(validator);
    ui->lineExamNumber2->setValidator(validator);
    ui->lineAppNumber->setFocus();

    QObject::connect(ui->pushButtonAddSubject, SIGNAL(clicked()), this, SLOT(viewDialog()));
    QObject::connect(ui->pushButtonAddSubject2, SIGNAL(clicked()), this, SLOT(viewDialog2()));
    QObject::connect(ui->pushButtonCancel, SIGNAL(clicked()), this, SLOT(clearWindow()));
    QObject::connect(ui->pushButtonSave, SIGNAL(clicked()), this, SLOT(acceptForm()));
    QObject::connect(ui->checkBoxTwoSittings, SIGNAL(toggled(bool)), this, SLOT(enableExams()));
    QObject::connect(ui->pushButtonRemove, SIGNAL(clicked()), this, SLOT(removeSubject()));
    QObject::connect(ui->pushButtonRemove2, SIGNAL(clicked()), this, SLOT(removeSubject2()));
}

Form::~Form()
{
    delete ui;
}

void Form::populateComboBoxes()
{
    QStringList listLevel, listSession, listDepartment, listReligion,
            listExamType, listSex, listMaritalStatus, listState;
    listState << "Abia" << "Osun" << "Ogun" << "Ondo" << "Ekiti" << "Lagos" << "Kwara"
              << "Oyo" << "Edo" << "Delta" << "Kano" << "Adamawa" << "Akwa Ibom"
              << "Anambra" << "Bauchi" << "Benue" << "Borno" << "Cross River" << "Enugu"
              << "Imo" << "Jigawa" << "Kaduna" << "Katsina" << "Kebbi" << "Kogi" << "Niger"
              << "Plateau" << "Rivers" << "Sokoto" << "Taraba" << "Yobe" << "Nassarawa";
    listState.sort();

    listReligion << "Christian" << "Muslim" << "Traditional" << "Other";
    listMaritalStatus << "Single" << "Married" << "Widow" << "Widower";
    listSex << "Male" << "Female";
    listLevel << "OND" << "HND";
    listDepartment << "Computer Science" << "SLT" << "Statistics" << "Food Tech";
    listDepartment.sort(Qt::CaseInsensitive);
    listSession << "2007/2008" << "2008/2009" << "2009/2010" << "2010/2011" << "2011/2012"
                << "2012/2013" << "2013/2014" << "2014/2015" << "2015/2016";
    listExamType << "SSCE" << "NECO" << "WAEC";

    ui->comboBoxDepartment->addItems(listDepartment);
    ui->comboBoxExamType->addItems(listExamType);
    ui->comboBoxExamType2->addItems(listExamType);
    ui->comboBoxSession->addItems(listSession);
    ui->comboBoxLevel->addItems(listLevel);
    ui->comboBoxSex->addItems(listSex);
    ui->comboBoxMaritalStatus->addItems(listMaritalStatus);
    ui->comboBoxReligion->addItems(listReligion);
    ui->comboBoxState->addItems(listState);
}

void Form::populateSpinBoxes()
{
    ui->spinBoxYear->setRange(1980, 2013);
    ui->spinBoxYear2->setRange(1980, 2013);
}

void Form::viewDialog()
{
    if(dialog->exec() == QDialog::Accepted){
        if(dialog->subject() == "" || dialog->grade() == "")
            return;
        ui->listWidget->addItem(dialog->subject() + " -- " + dialog->grade());
    }
}

void Form::viewDialog2()
{
    if(dialog->exec() == QDialog::Accepted){
        if(dialog->subject() == "" || dialog->grade() == "")
            return;
        ui->listWidget_2->addItem(dialog->subject() + " -- " + dialog->grade());
    }
}

bool Form::isSafeToClose()
{
    switch(QMessageBox::information(this, "Exit", "Are you sure you wanna exit?",
                                    QMessageBox::Yes | QMessageBox::No))
    {
    case QMessageBox::No:
        return false;
    default:
        return true;
    }
    return true;
}

void Form::acceptForm()
{
    QRegExp rep("^[a-zA-Z]+[^0-9]"), reps("[a-zA-Z]+\\s[a-zA-Z]+");
    if(rep.indexIn(ui->lineSurname->text().trimmed()) != 0 || reps.indexIn(ui->lineOthernames->text().trimmed()) != 0){
        QMessageBox::information(this, "Name", "Invalid Name given. Make sure \"Othernames\" are separated by spaces"
                                 " and does not contain any digit", QMessageBox::Ok);
        return;
    }
    if(ui->lineAppNumber->text() == "" || ui->lineSurname->text() == ""
            || ui->lineOthernames->text() == "" || ui->lineExamNumber->text() == ""
            || ui->lineAddress->text() == "")
    {
        QMessageBox::critical(this, "Error", "You cannot leave column(s) blankly",
                              QMessageBox::Ok);
    } else {
        if(ui->lineExamNumber->text() != ""){
            if(ui->listWidget->count() < 5){
                QMessageBox::critical(this, tr("Error!"), tr("You must have at least 5 O'Level subjects"),
                                      QMessageBox::Ok);
                viewDialog();
                return;
            }
        }
        if(ui->checkBoxTwoSittings->isChecked()){
            if(ui->lineExamNumber2->text() == "" || ui->listWidget_2->count() < 5){
                QMessageBox::critical(this, windowTitle(), tr("Make sure you fill in the exam year"
                                                              " and at least 5 subjects are chosen"),
                                      QMessageBox::Ok);
                return;
            } else {
                int allStudentsCount = ui->listWidget_2->count();
                for(int i = 0; i != allStudentsCount; ++i)
                {
                    studentList2.append(ui->listWidget_2->item(i)->text());
                }
                studentList2.insert(studentList2.begin(), ui->lineExamNumber2->text());
            }
        }

        QStringList databaseList;
        databaseList << ui->lineAppNumber->text().trimmed().toUpper() << ui->lineSurname->text().trimmed()
                     << ui->lineOthernames->text().trimmed() << ui->lineAddress->text().trimmed()
                     << ui->comboBoxSex->currentText() << ui->comboBoxLevel->currentText()
                     << ui->comboBoxSession->currentText() << ui->comboBoxMaritalStatus->currentText()
                     << ui->comboBoxReligion->currentText() << ui->comboBoxState->currentText()
                     << ui->comboBoxDepartment->currentText()
                     << ui->dateDOB->text() << ui->lineExamNumber->text().trimmed()
                     << ui->lineExamNumber2->text().trimmed() << ui->comboBoxExamType->currentText()
                     << ui->comboBoxExamType2->currentText() << ui->spinBoxYear->text()
                     << ui->spinBoxYear2->text();

        int allStudentsCounter = ui->listWidget->count();
        for(int i = 0; i != allStudentsCounter; ++i)
        {
            studentList.append(ui->listWidget->item(i)->text());
        }
        studentList.insert(studentList.begin(), ui->lineExamNumber->text());

        if(database){
            if(database.addNewStudent(databaseList, studentList, studentList2)
                    && generatePassword(databaseList[0]))
            {
                printRecord();
                QMessageBox::information(this, "Sucess", QString("Phase 1 completed. Close Window "
                                                                        "and \"Continue Registration\""),
                                         QMessageBox::Ok);
                this->close();
            } else {
                QMessageBox::information(this, tr("Error"),
                    tr("There was an error trying to save the record into the database. Please"
                       " check all inputs"), QMessageBox::Ok);
            }
        }
    }
}

void Form::clearWindow()
{
    ui->lineAppNumber->clear();
    ui->lineSurname->clear();
    ui->lineOthernames->clear();
    ui->lineAddress->clear();
    ui->listWidget->clear();
    ui->listWidget_2->clear();
    ui->lineExamNumber->clear();
    ui->lineExamNumber2->clear();

    ui->lineAppNumber->setFocus();
    ui->dateDOB->clear();
    ui->comboBoxState->setCurrentText("Abia");
}

void Form::enableExams()
{
    bool isCheckedTwoSittings = ui->checkBoxTwoSittings->isChecked();
    ui->lineExamNumber2->setEnabled(isCheckedTwoSittings);
    ui->listWidget_2->setEnabled(isCheckedTwoSittings);
    ui->comboBoxExamType2->setEnabled(isCheckedTwoSittings);
    ui->spinBoxYear2->setEnabled(isCheckedTwoSittings);
    ui->pushButtonAddSubject2->setEnabled(isCheckedTwoSittings);
    ui->pushButtonRemove2->setEnabled(isCheckedTwoSittings);
}

void Form::removeSubject()
{
    delete ui->listWidget->currentItem();
}

void Form::removeSubject2()
{
    delete ui->listWidget_2->currentItem();
}

bool Form::generatePassword(const QString &matric)
{
    LoginDialog login(1, this);
    if(login.exec() == QDialog::Accepted){
        if(login.isSame()){
            DatabaseHandler db;
            if(db){
                if(!db.addNewPassword({matric, login.getPassword()})){
                    QMessageBox::critical(nullptr, "Password", "An error occurred", QMessageBox::Ok);
                    generatePassword(matric);
                } else {
                    return true;
                }
            }
        } else {
            QMessageBox::critical(nullptr, "Password", "Passwords do not match", QMessageBox::Ok);
            generatePassword(matric);
        }
    } else {
        generatePassword(matric);
    }
    return true;
}

void Form::parse(QList<QPair<QString, QString>> &lists)
{
    QString middleName = (ui->lineOthernames->text().split(' ').size() >= 2) ?
                ui->lineOthernames->text().split(' ')[1]: ""
            , lastName = ui->lineOthernames->text().split(' ')[0];

    lists << Pair{"__appNumber__", ui->lineAppNumber->text()}
          << Pair{"__surname__", ui->lineSurname->text() };

    lists << Pair{ "__level(department)", QString("%1(%2)")
             .arg(ui->comboBoxLevel->currentText()).arg(ui->comboBoxDepartment->currentText())}
          << Pair{ "__session__", ui->comboBoxSession->currentText()}
          << Pair{ "__level__", (ui->comboBoxLevel->currentText() == "HND" ? "300": "100")};

    lists << Pair{"__dob__", ui->dateDOB->text() }
          << Pair{"__middleName__", middleName} << Pair{"__sex__", ui->comboBoxSex->currentText()}
          << Pair{"__firstName__", lastName}
          << Pair{"__marital__", ui->comboBoxMaritalStatus->currentText()} << Pair{"__state__", ui->comboBoxState->currentText()}
          << Pair{"__religion__", ui->comboBoxReligion->currentText()} << Pair{"__lga__", QString("%1").arg(ui->comboBoxState->currentText())};
}

void Form::printRecord()
{
    QMessageBox::information(this, "Save File", "A dialog box would show up in few second(s)\n"
                             "Choose the destination directory for your Kit", QMessageBox::Ok);
    QList<QPair<QString, QString>> lists;
    parse(lists);

    QString outFile;
    getOutputDestination(outFile);

    QFile iFile(":/HTML/result.html"), oFile(outFile);
    if(iFile.open(QIODevice::ReadOnly) && oFile.open(QIODevice::WriteOnly)){
        QTextStream cin(&iFile), cout(&oFile);
        while(!(cin.atEnd())){
            QString lines = cin.readLine();
            for(const auto &w: lists){
                lines.replace(w.first, w.second);
            }
            cout << lines;
        }
        iFile.close();
        oFile.close();
    } else {
        qDebug() << "Unable to open specified file";
        return;
    }
}

void Form::getOutputDestination(QString &outFile)
{
    outFile = QFileDialog::getSaveFileName(this, "Save your Kit", QDir::currentPath(), tr("HTML Files(*.html)"));
    if(outFile.isNull())
        getOutputDestination(outFile);

    if(!outFile.contains(".html"))
        outFile += QString(".html");

    m_filename = outFile;
}
