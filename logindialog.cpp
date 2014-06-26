#include "logindialog.h"

LoginDialog::LoginDialog(const int &_passwordField, QWidget *parent): QDialog(parent),
    checkGLayout(new QGridLayout(this)), username(new QLineEdit(this)),
    password(new QLineEdit(this)), login(new QPushButton("Login")),
    cancel(new QPushButton("Cancel"))
{
    populateAll(_passwordField);
    setLayout(checkGLayout);

    QObject::connect(username, SIGNAL(textChanged(QString)), this, SLOT(setUsername(QString)));
    QObject::connect(password, SIGNAL(textChanged(QString)), this, SLOT(setPassword(QString)));

    QObject::connect(login, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::connect(cancel, SIGNAL(clicked()), this, SLOT(close()));
}

bool LoginDialog::checkLogin(const QString &m_username, const QString &m_password)
{
    DatabaseHandler database;
    if(database){
        if(database.checkLoginStatus(QPair<QString, QString>(m_username, m_password))){
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

void LoginDialog::setUsername(const QString &text)
{
    if(textUsername == text)
        return;

    textUsername = text;
}

void LoginDialog::setPassword(const QString &text)
{
    if(textPassword == text)
        return;

    textPassword = text;
}

const QString &LoginDialog::getUsername() const
{
    return textUsername;
}

const QString&LoginDialog::getPassword() const
{
    return textPassword;
}

bool LoginDialog::isSame()
{
    return textUsername == textPassword;
}

void LoginDialog::populateAll(const int &setPasswordField)
{
    username->clear();
    password->clear();
    username->setFocus();
    if(setPasswordField){
        labelUsername = new QLabel("Password");
        labelPassword = new QLabel("Repeat Password");
        username->setEchoMode(QLineEdit::Password);
    } else {
        labelUsername = new QLabel("Username");
        labelPassword = new QLabel("Password");
    }
    checkGLayout->addWidget(labelUsername, 0, 0);
    checkGLayout->addWidget(labelPassword, 1, 0);

    password->setEchoMode(QLineEdit::Password);

    checkGLayout->addWidget(username, 0, 1);
    checkGLayout->addWidget(password, 1, 1);
    checkGLayout->addWidget(login, 2, 0);
    checkGLayout->addWidget(cancel, 2, 1);
}

bool LoginDialog::loginAdmin(const QString &user, const QString &pass)
{
    DatabaseHandler db;
    if(db)
        return db.loginAdmin(user, pass);
    return false;
}
