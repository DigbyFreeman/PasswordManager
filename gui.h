#ifndef PASSWORDMANAGER_H
#define PASSWORDMANAGER_H

#include "./ui_gui.h"
#include <QDebug>
#include <QMainWindow>
#include "functions.h"
#include <QGraphicsOpacityEffect>
#include <QTimer>
#include <QPropertyAnimation>
#include <QStackedWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class passwordManager; }
QT_END_NAMESPACE

class passwordManager : public QMainWindow
{
    Q_OBJECT

public:
    login Login;
    Database database;
    passwordManager(QWidget *parent = nullptr);
    ~passwordManager();
    QTimer* timer = new QTimer(this);
    QPropertyAnimation* animation;
    QGraphicsOpacityEffect* effect;
    void display(const char* message);


private slots:
    void onLoginButton_clicked();
    void onSignupButton_clicked();
    void delete_clicked();
    void addNew_clicked();
    void edit_clicked();
    void logout_clicked();
    void confirm_clicked();
    void go_back_clicked();
    void updateConfirm_clicked();
    void updateGo_back_clicked();
    void onMainUsernameField_Change(const QString &text);
    void onMainPasswordField_Change(const QString &text);
    void usernameField(const QString &text);
    void passwordField(const QString &text);
    void websiteField(const QString &text);
    void showPasswordBox();
    void displayDetails(QListWidgetItem* item);



private:
    Ui::passwordManager *ui;
    std::vector<std::vector<std::string> > results;
    void showInformation(bool displayAll=false);

};
#endif // PASSWORDMANAGER_H
