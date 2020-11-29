#include "data.h"

#include <QtSql>

Data::Data(QObject *parent) : QObject(parent)
{
    date = QDateTime();
}

Data::Data(QObject *parent, QSqlQuery &qry) :
    QObject(parent)
{
    this->id = qry.value("id").toInt();
    this->code_all = qry.value("code_all").toInt();
    this->code_year = qry.value("code_year").toInt();
    this->date = qry.value("date").toDateTime();
    this->person = qry.value("person").toString();
    this->density = qry.value("density").toInt();
    this->juice = qry.value("juice").toInt();
    this->type = qry.value("type").toString();
    this->start = qry.value("start").toTime();
    this->end = qry.value(("end")).toTime();
    this->dimensionFrom = qry.value("dimFrom").toInt();
    this->dimensionTo = qry.value("dimTo").toInt();
    this->temperature = qry.value("temperature").toFloat();
    this->description = qry.value("description").toString();
}


QVariant Data::Id()
{
    return id;
}

bool Data::isNew()
{
    if(!id.isValid())return true;//Id не валидный, значит нулевой
    if(id.isNull()) return true;//Указатель нулевой, значит новый элемент
    return false;
}

int Data::Code_all()
{
    return this->code_all;
}

void Data::setCode_all(int value)
{
    this->code_all = value;
}

int Data::Code_year()
{
    return this->code_year;
}

void Data::setCode_year(int value)
{
    this->code_year = value;
}

QDateTime Data::Date()
{
    return this->date;
}

QString Data::Date_month()
{
    QString month;

    if(this->date.toString("MM") == "01"){
        month = "января";
    } else if(this->date.toString("MM") == "02"){
        month = "февраля";
    } else if(this->date.toString("MM") == "03"){
        month = "марта";
    } else if(this->date.toString("MM") == "04"){
        month = "апреля";
    } else if(this->date.toString("MM") == "05"){
        month = "мая";
    } else if(this->date.toString("MM") == "06"){
        month = "июня";
    } else if(this->date.toString("MM") == "07"){
        month = "июля";
    } else if(this->date.toString("MM") == "08"){
        month = "августа";
    } else if(this->date.toString("MM") == "09"){
        month = "сентября";
    } else if(this->date.toString("MM") == "10"){
        month = "октября";
    } else if(this->date.toString("MM") == "11"){
        month = "ноября";
    } else if(this->date.toString("MM") == "12"){
        month = "декабря";
    }

    return month;
}

QString Data::Date_day()
{
    //Если день начинается на 0, например "07", то берём только правую часть от даты
    //иначе не меняем день
    QString day;

    if(this->date.toString("dd").left(1) == "0"){
        day = this->date.toString("dd").right(1);
    } else {
        day = this->date.toString("dd");
    }

    return day;
}

void Data::setDate(QDateTime value)
{
    this->date = value;
}

QString Data::Person()
{
    return this->person;
}

void Data::setPerson(QString person)
{
    this->person = person;
}

int Data::Density()
{
    return this->density;
}

QString Data::Density_plus()
{
    QString pasta;

    if(this->Type() == "Sugar"){
    if(this->density == 100){
        pasta = "0 [115]";
    } else if(this->density  == 90){
        pasta = "1 [105]";
    } else if(this->density  == 80){
        pasta = "2 [95]";
    } else if(this->density  == 70){
        pasta = "3 [85]";
    } else if(this->density  == 60){
        pasta = "4 [75]";
    } else if(this->density  == 50){
        pasta = "5 [65]";
    }
    } else {
        if(this->density == 100){
            pasta = "0 [100]";
        } else if(this->density  == 90){
            pasta = "1 [90]";
        } else if(this->density  == 80){
            pasta = "2 [80]";
        } else if(this->density  == 70){
            pasta = "3 [70]";
        } else if(this->density  == 60){
            pasta = "4 [60]";
        } else if(this->density  == 50){
            pasta = "5 [50]";
        }
    }

    return pasta;
}

QString Data::Density_display()
{
    QString display;

    if(this->density == 100){
        display = "0 (bandage)";
    } else if(this->density  == 90){
        display = "1 (ultrasoft)";
    } else if(this->density  == 80){
        display = "2 (soft)";
    } else if(this->density  == 70){
        display = "3 (medium)";
    } else if(this->density  == 60){
        display = "4 (hard)";
    } else if(this->density  == 50){
        display = "5 (extrahard)";
    }

    return display;
}


void Data::setDensity(int value)
{
    this->density = value;
}

int Data::Juice()
{
    return this->juice;
}

void Data::setJuice(int value)
{
    this->juice = value;
}

QString Data::Type()
{
    return this->type;
}

void Data::setType(QString type)
{
    this->type = type;
}

QTime Data::Start()
{
    return this->start;
}

void Data::setStart(QTime value)
{
    this->start = value;
}

QTime Data::End()
{
    return this->end;
}

void Data::setEnd(QTime value)
{
    this->end = value;
}

QString Data::secondsToString()
{
    QString result;
    QTime timeZero = QTime(0, 0, 0);
    float timeDelta = this->start.secsTo(this->end);

    if(timeDelta < 0){
        float timeDelta1 = 86400 - timeZero.secsTo(this->start);
        float timeDelta2 = timeZero.secsTo(this->end);
        timeDelta = timeDelta1 + timeDelta2;
    }

    int a; //числитель
    int b; //знаменатель int
    float d;//знаменатель float

    //округляем
    int c = (timeDelta/3600) * 100;
    //получаем числитель
    a = (c / 100) % 100;
    //получем знаменатель
    b = (c % 100) * 0.6;
    d = (c % 100) * 0.6;

    //Поправка, для точного округления
    if(d - b > 0.5){
        b += 1;
    }

    if(b < 10){
        result = QString::number(a) + ":0" + QString::number(b);
    } else {
        result = QString::number(a) + ":" + QString::number(b);
    }

    return result;
}


int Data::DimensionFrom()
{
    return this->dimensionFrom;
}

void Data::setDimensionFrom(int value)
{
    this->dimensionFrom = value;
}

int Data::DimensionTo()
{
    return this->dimensionTo;
}

void Data::setDimensionTo(int value)
{
    this->dimensionTo = value;
}

QString Data::dimensionsToString()
{
    QString result;

    if(this->dimensionFrom != 0){
        result = "(" + QString::number(this->dimensionFrom) + "-" +
                QString::number(this->dimensionTo) + ")" +
                QString::number(this->temperature) + "°С";
    } else {
        result = "нет";
    }
    return result;
}

float Data::Temperature()
{
    return this->temperature;
}

void Data::setTemperature(float value)
{
    this->temperature = value;
}

QString Data::Description()
{
    return this->description;
}

void Data::setDescription(QString text)
{
    this->description = text;
}

QString Data::someText()
{
    QString text;
    text = "0 - 50, 1 - 36, 2 - 24, 3 - 16, 4 - 12. Temp - 22°С";
    return text;
}

QString Data::Report()
{
    QString report;

    //Для глюкозно-фруктозных паст
    if(this->juice != 0){
        report = QString::number(this->juice)  + " " +
                this->start.toString("hh.mm") + " " +
                this->end.toString("hh.mm") + " " +
                this->description + " " +
                QString::number(this->code_all) + " " +
                QString::number(this->code_year);
    } else {//Для паст на сахаре
        report =  this->start.toString("hh.mm") + " " +
                this->end.toString("hh.mm") + " " +
                this->description + " " +
                QString::number(this->code_all) + " " +
                QString::number(this->code_year);
    }

    return report;
}


