#include "gui.h"

passwordManager::passwordManager(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::passwordManager)
{
    ui->setupUi(this);
    //Making grid layour centre
    ui->mainUsernameField->setPlaceholderText("Enter username");
    ui->mainPasswordField->setPlaceholderText("Enter password");
    ui->mainPasswordField->setEchoMode(QLineEdit::Password);
    ui->stackedWidget->setCurrentIndex(0);
    QSizePolicy sp_retain = ui->stackedWidget->sizePolicy();
    sp_retain.setRetainSizeWhenHidden(true);
    ui->stackedWidget->setSizePolicy(sp_retain);


    ui->titleText->setStyleSheet("background-color: transparent;"); // set background to transparent
    ui->titleText->setAlignment(Qt::AlignCenter); // align text to center
    QTextCursor cursor = ui->titleText->textCursor();
    QTextCharFormat format;
    format.setFontPointSize(18); // set the font size to 18 points
    cursor.select(QTextCursor::Document);
    cursor.setCharFormat(format);
    QTextBlockFormat blockFormat;
    blockFormat.setAlignment(Qt::AlignCenter);
    cursor.setBlockFormat(blockFormat);
    ui->titleText->setTextCursor(cursor);
    ui->titleText_2->setStyleSheet("background-color: transparent;");

    ui->errorAuthDisplay->setStyleSheet("background-color: transparent;");
    effect = new QGraphicsOpacityEffect(this);
    ui->errorAuthDisplay->setGraphicsEffect(effect);
    ui->errorAuthDisplay->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // Disable vertical scrollbar

    connect(ui->loginButton, SIGNAL(clicked()), this, SLOT(onLoginButton_clicked()));
    connect(ui->signupButton, SIGNAL(clicked()), this, SLOT(onSignupButton_clicked()));
    connect(ui->Delete, SIGNAL(clicked()), this, SLOT(delete_clicked()));
    connect(ui->Edit, SIGNAL(clicked()), this, SLOT(edit_clicked()));
    connect(ui->AddNew, SIGNAL(clicked()), this, SLOT(addNew_clicked()));
    connect(ui->logoutButton, SIGNAL(clicked()), this, SLOT(logout_clicked()));
    connect(ui->Confirm, SIGNAL(clicked()), this, SLOT(confirm_clicked()));
    connect(ui->Go_Back, SIGNAL(clicked()), this, SLOT(go_back_clicked()));
    connect(ui->updateConfirm, SIGNAL(clicked()), this, SLOT(updateConfirm_clicked()));
    connect(ui->updateGoBack, SIGNAL(clicked()), this, SLOT(updateGo_back_clicked()));
    connect(ui->checkBox, SIGNAL(clicked()), this, SLOT(showPasswordBox()));
    connect(ui->showAccount, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(displayDetails(QListWidgetItem*)));
}

void passwordManager::display(const char* message) {
    ui->errorAuthDisplay->append(message);

    // Stop any previous animations if they're still running
    if (animation && animation->state() == QAbstractAnimation::Running) {
        animation->stop();
    }

    // Create a QGraphicsOpacityEffect for the errorAuthDisplay
    QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect(this);
    ui->errorAuthDisplay->setGraphicsEffect(effect);

    // Set the initial opacity to 1.0 (fully visible)
    effect->setOpacity(1.0);

    // Create a new animation and connect it to the effect
    animation = new QPropertyAnimation(effect, "opacity", this);
    animation->setDuration(1000);
    animation->setStartValue(1.0);
    animation->setEndValue(0.0);


    // Clear the errorAuthDisplay once the animation is finished
    connect(animation, &QPropertyAnimation::finished, this, [=]() mutable {
        ui->errorAuthDisplay->clear();
        animation = nullptr; // Set animation to nullptr when it's finished
    });

    // Start the animation after a 3-second delay
    timer->start(3000); // 3 seconds
    connect(timer, &QTimer::timeout, this, [=]() {
            if (animation) {
                animation->start(QAbstractAnimation::DeleteWhenStopped);
            }
        }, Qt::QueuedConnection);
}


passwordManager::~passwordManager()
{
    delete ui;
}

void passwordManager::showInformation(bool displayAll) {
    ui->showAccount->clear();
    database.select(Login.shaPassword,results,false);
    for (const auto& row : results) {
        QListWidgetItem* item = new QListWidgetItem();
        QStringList rowText;
        if (displayAll) {
            if (row.size() > 1) {
                rowText << QString("%1 - %2 - %3 - %4").arg(QString::fromStdString(row[0]), QString::fromStdString(row[1]), QString::fromStdString(row[2]), QString::fromStdString(row[3]));
            }
        }
        else if (row.size() > 1) {
            rowText << QString("%1 - %2").arg(QString::fromStdString(row[0]), QString::fromStdString(row[1])); // Display both the username and ID
        }
        item->setText(rowText.join(" "));
        ui->showAccount->addItem(item);
    }
}


void passwordManager::onLoginButton_clicked()
{
    if (ui->mainUsernameField->text() != "" && ui->mainPasswordField->text() != "" && ui->mainUsernameField->text() != ui->mainUsernameField->placeholderText() && ui->mainPasswordField->text() != ui->mainPasswordField->placeholderText()) {
        QString username = ui->mainUsernameField->text();
        QString password = ui->mainPasswordField->text();
        Login.authenticate((unsigned char*)((username.toUtf8()).data()),(unsigned char*)((password.toUtf8()).data()),&database);
        if (Login.hasAuthenticated()) {
            ui->stackedWidget->setCurrentIndex(1);
            showInformation();
        }
        else {
            display("Error: inputted account details may be incorrect");
        }
    }
}

void passwordManager::onSignupButton_clicked()
{
    if (ui->mainUsernameField->text() != "" && ui->mainPasswordField->text() != "" && ui->mainUsernameField->text() != ui->mainUsernameField->placeholderText() && ui->mainPasswordField->text() != ui->mainPasswordField->placeholderText()) {
        QString username = ui->mainUsernameField->text();
        QString password = ui->mainPasswordField->text();
        Login.registerUser((unsigned char*)((username.toUtf8()).data()),(unsigned char*)((password.toUtf8()).data()),&database);
        if (Login.hasAuthenticated()) {
            ui->stackedWidget->setCurrentIndex(1);
            showInformation();
        }
        else {
            display("Error: account details may not be long enough.\n"
                                     "Username must be 5 or more characters.\n"
                                     "Password must be 8 or more characters.");
        }
    }
}

void passwordManager::onMainUsernameField_Change(const QString &text)
{
    if (text == ui->mainUsernameField->placeholderText()) {
        ui->mainUsernameField->clear();
    }
}

void passwordManager::onMainPasswordField_Change(const QString &text)
{
    if (text == ui->mainPasswordField->placeholderText()) {
        ui->mainPasswordField->clear();
    }
}

void passwordManager::usernameField(const QString &text)
{
    if (text == ui->Username->placeholderText()) {
        ui->Username->clear();
    }
}

void passwordManager::passwordField(const QString &text)
{
    if (text == ui->Password->placeholderText()) {
        ui->Password->clear();
    }
}

void passwordManager::websiteField(const QString &text)
{
    if (text == ui->Website->placeholderText()) {
        ui->Website->clear();
    }
}

void passwordManager::showPasswordBox()
{
    if (ui->checkBox->isChecked()) {
        ui->mainPasswordField->setEchoMode(QLineEdit::Normal);
    } else {
        ui->mainPasswordField->setEchoMode(QLineEdit::Password);
    }
}

void passwordManager::edit_clicked() {
    ui->stackedWidget->setCurrentIndex(3);
}

void passwordManager::addNew_clicked() {
    ui->stackedWidget->setCurrentIndex(2);
}

void passwordManager::delete_clicked() {
    try {
        QListWidgetItem *firstItem = ui->displayDetails->item(0);
        QStringList temp = firstItem->text().split(": ");
        if (temp[0] == "ID") {
            int id = firstItem->text().split(": ")[1].toInt();
            database.deleteRecord(id);
            showInformation();
            ui->displayDetails->clear();
        }
        else {
            display("Error deleting record.");
        }
    }
    catch (const std::exception& ex) {
        display("Error in delete function.");
    }
}


void passwordManager::logout_clicked() {
    Login.logout();
    database.database = nullptr;
    ui->stackedWidget->setCurrentIndex(0);
    ui->mainUsernameField->clear();
    ui->mainPasswordField->clear();
    ui->checkBox->setChecked(false);
}

void passwordManager::confirm_clicked() {
    const char* username = (const char*)((ui->Username->text().toStdString()).c_str());
    const char* password = (const char*)((ui->Password->text().toStdString()).c_str());
    const char* website = (const char*)((ui->Website->text().toStdString()).c_str());
    int i = database.insert(password,Login.shaPassword,username,website);
    if (i) {
        display("Error: new user details not inputted into database. Username and website must be 5 characters long, and password must be 8 minimum.");
    }
    else {
        display("Success! New user details added to database!");
    }
    showInformation();
}

void passwordManager::go_back_clicked() {
    ui->stackedWidget->setCurrentIndex(1);
}

void passwordManager::updateConfirm_clicked() {
    int id = ui->updateID->text().toInt();
    const char* username = (const char*)((ui->updateUsername->text().toStdString()).c_str());
    const char* password = (const char*)((ui->updatePassword->text().toStdString()).c_str());
    const char* website = (const char*)((ui->updateWebsite->text().toStdString()).c_str());
    int p = database.update(id, pass,Login.shaPassword,password,username,website);
    int o = database.update(id, user,Login.shaPassword,password,username,website);
    int i = database.update(id, site,Login.shaPassword,password,username,website);
    if (i || o || p) {
        display("Error inputting some details");
    }
    display("Assume atleast one worked.");
    showInformation();
}

void passwordManager::updateGo_back_clicked() {
    ui->stackedWidget->setCurrentIndex(1);
}


void passwordManager::displayDetails(QListWidgetItem *item) {
    // Clear the current items in displayDetails
    ui->displayDetails->clear();
    // Get the selected row's data
    QStringList rowData = item->text().split(" - ");
    showInformation(true);
    for (QListWidgetItem* item : ui->showAccount->findItems(QString("*"), Qt::MatchWildcard)) {
        if (rowData[0] == item->text().split(" - ")[0]) {
            rowData = item->text().split(" - ");
        }
    }
    showInformation();

    QListWidgetItem *item1 = new QListWidgetItem("ID: " + rowData[0]);
    QListWidgetItem *item2 = new QListWidgetItem("Website: " + rowData[1]);
    QListWidgetItem *item3 = new QListWidgetItem("Username: " + rowData[2]);
    QListWidgetItem *item4 = new QListWidgetItem("Password: " + rowData[3]);

    ui->displayDetails->addItem(item1);
    ui->displayDetails->addItem(item2);
    ui->displayDetails->addItem(item3);
    ui->displayDetails->addItem(item4);
}


