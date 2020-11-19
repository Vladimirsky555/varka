#include "application.h"

#include <QtSql>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QFile>


Application::Application(int argc, char *argv[]) : QApplication(argc, argv)
{
    connectToDataBase();
}

Application::~Application(){}

void Application::connectToDataBase()
{
    /* Перед подключением к базе данных производим проверку на её существование.
     * В зависимости от результата производим открытие базы данных или её восстановление
     * */
    if(!QFile("data.db").exists()){
        this->restoreDataBase();
    } else {
        this->openDataBase();
    }
}

bool Application::openDataBase()
{
    /* База данных открывается по заданному пути
        * и имени базы данных, если она существует
        * */

       db = QSqlDatabase::addDatabase("QSQLITE");
       db.setHostName("varkaDB");
       db.setDatabaseName("data.db");
       if(db.open()){
           return true;
       } else {
           return false;
       }
}

bool Application::restoreDataBase()
{
    if(this->openDataBase()){
        return (this->createTable()) ? true : false;
    } else {
        QMessageBox::critical(NULL, "Сообщение!",
                              "Не удалось восстановить базу данных!");
        return false;
    }
    return false;
}

void Application::closeDataBase()
{
    db.close();
}

bool Application::createTable()
{
    QSqlQuery query;
    if(!query.exec( "CREATE TABLE  varkaDB  ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "code_all VARCHAR(255),"
                    "code_year VARCHAR(255),"
                    "date TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,"
                    "person   VARCHAR(255),"
                    "density     INT(4),"
                    "juice INT(4),"
                    "type   VARCHAR(255),"
                    "start TIMESTAMP,"
                    "end TIMESTAMP,"
                    "dimFrom INT(4),"
                    "dimTo INT(4),"
                    "temperature REAL,"
                    "description  TEXT"
                    " )"


                    )){
        QMessageBox::critical(NULL, "Сообщение!",
                              "Ошибка создания базы данных! " +
                              query.lastError().text());

        return false;
    } else {
        return true;
    }
    return false;
}



