#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include <pqxx/pqxx>
#include <QIntValidator>
#include <QTableWidget>
#include <QTableWidgetItem>
pqxx::connection *conn = nullptr;

void MainWindow::connectToDB(){
    try {
        if(conn == nullptr){
            conn = new pqxx::connection("dbname=SGMS user=postgres password=password");
        }
    } catch (const std::exception &e) {
        QMessageBox::warning(this, "Error" , e.what());
        throw;
    }
}

void MainWindow::disconnectDB(){
    if(conn!=nullptr){
        delete conn;
        conn = nullptr;
    }
}

void MainWindow::navigateToHomePage(){
    ui->stackedWidget->setCurrentIndex(0);
}

std::string MainWindow::getGrade(int score){
    if(score >= AGRADE_MIN && score <= AGRADE_MAX){
        return "A";
    }else if(score >= BGRADE_MIN && score <= BGRADE_MAX){
        return "B";
    }else {
        return "C";
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setStyleSheet("background-color: #36454F");

    ui->stackedWidget->setCurrentIndex(0);

    ui->studentId->setPlaceholderText("student ID");
    ui->Age->setPlaceholderText("Age");
    ui->username->setPlaceholderText("Username");
    ui->gradeLevel->setPlaceholderText("Grade Level (1-12)");

    ui->Age->setValidator(new QIntValidator(1, 100, this));
    ui->gradeLevel->setValidator(new QIntValidator(1, 12, this));

    //Navigating to home pages using signals and slots
    connect(ui->Home, &QPushButton::clicked, this, &MainWindow::navigateToHomePage);
    connect(ui->Home2, &QPushButton::clicked, this, &MainWindow::navigateToHomePage);
    connect(ui->Home3, &QPushButton::clicked, this, &MainWindow::navigateToHomePage);
    connect(ui->Home4, &QPushButton::clicked, this, &MainWindow::navigateToHomePage);
    connect(ui->Home5, &QPushButton::clicked, this, &MainWindow::navigateToHomePage);

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_addST_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}


void MainWindow::on_ADD_clicked()
{
    QString sid = ui->studentId->text();
    QString uname = ui->username->text();
    QString ageText = ui->Age->text();
    QString glText = ui->gradeLevel->text();

    if(sid.isEmpty() || uname.isEmpty() || ageText.isEmpty() || glText.isEmpty()){
        QMessageBox::warning(this, "Error", "please fill in all the required fields");
        return;
    }

    try{
        MainWindow::connectToDB();

        pqxx::work W(*conn);
        std::string query = "INSERT INTO students (stid, username, age, gradelevel) VALUES (" +
                            W.quote(sid.toStdString()) + ", " +
                            W.quote(uname.toStdString()) + ", " +
                            W.quote(ageText.toStdString()) + ", " +
                            W.quote(glText.toStdString()) + ")";
        W.exec(query);
        W.commit();
        QMessageBox::information(this, "Success", "Student details inserted successfully");
    }catch(const std::exception &e){
        qDebug() << "Error: " << e.what();
        QMessageBox::warning(this, "Error", "Database operation failed");
    }

    for(QLineEdit *lineEdit : this->findChildren<QLineEdit*>()){
        lineEdit->clear();
    }
    MainWindow::disconnectDB();

}

void MainWindow::on_addGRD_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
    ui->grdid->setPlaceholderText("Student ID");
    ui->AI->setPlaceholderText("AI score");
    ui->CS->setPlaceholderText("Cyber Security Score");
    ui->DS->setPlaceholderText("Data Science Score");
    ui->PM->setPlaceholderText("Psychology of Money score");

    ui->AI->setValidator(new QIntValidator(0,100,this));
    ui->CS->setValidator(new QIntValidator(0,100,this));
    ui->DS->setValidator(new QIntValidator(0,100,this));
    ui->PM->setValidator(new QIntValidator(0,100,this));
}


void MainWindow::on_add2_clicked()
{
    QString grdid = ui->grdid->text();
    QString aiText = ui->AI->text();
    QString csText = ui->CS->text();
    QString dsText = ui->DS->text();
    QString pmText = ui->PM->text();

    if(grdid.isEmpty() || aiText.isEmpty() || csText.isEmpty() || dsText.isEmpty() || pmText.isEmpty()){
        QMessageBox::warning(this, "Error", "Please fill in all the required fields");
        return;
    }

    try{
        MainWindow::connectToDB();
        pqxx::work w(*conn);

        pqxx::result validId = w.exec("select * from students where stid = " + w.quote(grdid.toStdString()));
        if(!validId.empty()){
            pqxx::result idExist = w.exec("select * from grades where stid =" + w.quote(grdid.toStdString()));
            if (!idExist.empty()) {
                QMessageBox::information(this, "Failure", "Grades record exists for the given ID");
            } else {
                w.exec("INSERT INTO grades (stid, AI, CS, DS, PM) VALUES (" +
                       w.quote(grdid.toStdString()) + ", " +
                       w.quote(MainWindow::getGrade(aiText.toInt())) + ", " +
                       w.quote(MainWindow::getGrade(csText.toInt())) + ", " +
                       w.quote(MainWindow::getGrade(dsText.toInt())) + ", " +
                       w.quote(MainWindow::getGrade(pmText.toInt())) + ")");
                QMessageBox::information(this, "Success", "Grades added successfully for the student id " + grdid);
            }

            w.commit();
        }else{
            QMessageBox::warning(this, "Error", "Student with the given ID is not found in the student database.");
        }
    }catch(const std::exception &e){
        qDebug() << "Error: " << e.what();
        QMessageBox::warning(this, "Error", "Database operation failed");
    }

    for (QLineEdit *lineEdit : this->findChildren<QLineEdit*>()) {
        lineEdit->clear();
    }
    MainWindow::disconnectDB();
}


void MainWindow::on_search_clicked()
{
    QString sid = ui->searchId->text();
    try{
        MainWindow::connectToDB();
        pqxx::work w(*conn);

        pqxx::result res = w.exec_params("select * from students where stid = $1", sid.toStdString());
        if(!res.empty()) {
            const auto &row = res[0];
            QString studentId = QString::fromStdString(row["stid"].as<std::string>());
            QString name = QString::fromStdString(row["username"].as<std::string>());

            QMessageBox::information(this, "Success", studentId + " exists with the name " + name);
        }else{
            QMessageBox::warning(this, "Error", "No student exists with this ID.");
        }

    }catch(const std::exception &e){
        qDebug() << "Error: " << e.what();
        QMessageBox::warning(this, "Failure", "Database operation failed.");
    }

    for (QLineEdit *lineEdit : this->findChildren<QLineEdit*>()) {
        lineEdit->clear();
    }
    MainWindow::disconnectDB();
}


void MainWindow::on_searchST_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
    ui->searchId->setPlaceholderText("Student ID");
}


void MainWindow::on_listST_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);

    try{
        MainWindow::connectToDB();
        pqxx::work w(*conn);
        pqxx::result res = w.exec("select * from students");

        ui->tableWidget->clear();
        ui->tableWidget->setColumnCount(4);
        ui->tableWidget->setHorizontalHeaderLabels({"studentId", "Name", "Age", "GradeLevel"});
        ui->tableWidget->setRowCount(res.size());

        int row = 0;
        for(const auto &r: res){
            ui->tableWidget->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(r["stid"].as<std::string>())));
            ui->tableWidget->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(r["username"].as<std::string>())));
            ui->tableWidget->setItem(row, 2, new QTableWidgetItem(QString::number(r["age"].as<int>())));
            ui->tableWidget->setItem(row, 3, new QTableWidgetItem(QString::number(r["gradelevel"].as<int>())));
            row++;
        }
        w.commit();

    }catch(const std::exception &e){
        qDebug() << "Error: " << e.what();
        QMessageBox::warning(this, "Failure" , "Database operation failed");
    }
    MainWindow::disconnectDB();
}


void MainWindow::on_STreport_clicked()
{
    ui->stackedWidget->setCurrentIndex(5);
    ui->reportId->setPlaceholderText("Student Id");
}


void MainWindow::on_pushButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(6);

    std::string id, AI, CS, DS, PM;
    int totCredits = 0, productSum = 0;
    std::map<std::string, int> subMap {{"A",10}, {"B", 8}, {"C", 6}};
    std::vector<std::string> grd;

    QString resText = "Student Report \n";
    resText += "---------------- \n\n\n";

    QString reportId = ui->reportId->text();

    try{
        MainWindow::connectToDB();
        pqxx::work w(*conn);
        pqxx::result res = w.exec_params("select * from students where stid = $1", reportId.toStdString());
        if(!res.empty()) {
            for (const auto &row: res){
                resText += "Name: " + QString::fromStdString(row[1].c_str()) + "\n";
                resText += "ID: " + QString::fromStdString(row[0].c_str()) + "\n";
                resText += "Grade Level: " + QString::number(row[3].as<int>()) + "\n\n";
            }
            resText += "Subjects: \n";
            pqxx::result sub = w.exec_params("select * from grades where stid = $1", reportId.toStdString());
            if(sub.empty()){
                QMessageBox::warning(this, "Failure" ,"Grades are not available for this student id");
            }else{
                pqxx::result credits = w.exec_params("select credits from subjects");
                for (const auto &rw: sub){
                    AI = rw[1].c_str();
                    CS = rw[2].c_str();
                    DS = rw[3].c_str();
                    PM = rw[4].c_str();
                    resText += "1. AI: (" + AI +")\n";
                    resText += "2. Cyber Security: (" + CS + ")\n";
                    resText += "3. Data Science: (" +  DS + ")\n";
                    resText += "4. Psychology of money: (" + PM + ")\n\n";
                    grd.push_back(AI);
                    grd.push_back(CS);
                    grd.push_back(DS);
                    grd.push_back(PM);
                }
                int i = 0;
                for(const auto &r: credits){
                    totCredits += r["credits"].as<int>();
                    if(grd[i] != "\0"){
                        productSum += ((subMap.at(grd[i])) * (r["credits"].as<int>()));
                    }
                    i++;
                }
                float gpa = static_cast<float>(productSum)/totCredits;
                resText += "Overall Average: " + QString::number(productSum) + "\n";
                resText += "GPA: " + QString::number(gpa, 'f', 2) + "\n";
            }
        }else{
            QMessageBox::warning(this, "Failure", "No student exists with that ID, Please recheck");
            return;
        }
        w.commit();
    }catch(const std::exception &e){
        qDebug() << "Error: " << e.what();
        QMessageBox::warning(this, "Failure", "Database Operation Failed");
    }
    ui->plainTextEdit->setPlainText(resText);
    MainWindow::disconnectDB();
}

