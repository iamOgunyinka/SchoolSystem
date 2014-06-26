#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include <QtWidgets>
#include "dialog.h"
#include "logindialog.h"

namespace Ui {
class Form;
}

class Form : public QWidget
{
    Q_OBJECT
    void printRecord();
    void getOutputDestination(QString &outFile);
public:
    typedef QPair<QString, QString> Pair;
private:
    void parse(QList<Pair> &lists);
public slots:
    void viewDialog();
    void viewDialog2();
    void acceptForm();
    void clearWindow();
    void enableExams();
    void removeSubject();
    void removeSubject2();
public:
    explicit Form(QString &filename, QWidget *parent = 0);
    ~Form();
    void populateComboBoxes();
    void populateSpinBoxes();
    bool generatePassword(const QString &matric);
private:
    QStringList studentList, studentList2;
    Ui::Form *ui;
    Dialog *dialog;
    bool isSafeToClose();
    DatabaseHandler database;
    QString &m_filename;
};

#endif // FORM_H
