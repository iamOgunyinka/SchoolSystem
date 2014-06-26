#ifndef STUDENTRESULT_H
#define STUDENTRESULT_H

#include <QDialog>
#include "databasehandler.h"
#include "dialog.h"

namespace Ui {
class StudentResult;
}

class StudentResult : public QDialog
{
    Q_OBJECT
    
    void openDialog(const QStringList &courses);
public slots:
    void pullUpCourses();
    void removeCourse();
public:
    explicit StudentResult(const QStringList &record, QDialog *parent = nullptr);
    ~StudentResult();
    void populateComboBoxes();
private:
    Ui::StudentResult *ui;
    QStringList localRecord;
};

#endif // STUDENTRESULT_H
