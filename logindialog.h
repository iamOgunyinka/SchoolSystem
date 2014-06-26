#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include "databasehandler.h"

class LoginDialog: public QDialog
{
    Q_OBJECT

    void populateAll(const int &setPasswordField);
public:
    LoginDialog(const int &_passwordField = 0, QWidget *parent = nullptr);
    const QString &getUsername() const;
    const QString &getPassword() const;
    bool isSame();
    bool loginAdmin(const QString &user, const QString &pass);
public slots:
    bool checkLogin(const QString &m_username, const QString &m_password);
    void setUsername(const QString &text);
    void setPassword(const QString &text);
private:
    QGridLayout *checkGLayout;
    QLineEdit *username, *password;
    QPushButton *login, *cancel;
    QLabel *labelUsername, *labelPassword;
    QString textUsername, textPassword;
};

#endif // LOGINDIALOG_H
