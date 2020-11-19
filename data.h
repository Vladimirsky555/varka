#ifndef DATA_H
#define DATA_H

#include <QObject>
#include <QVariant>
#include <QDateTime>
#include <QTime>
#include <QVariant>

class QSqlQuery;

class Data : public QObject
{
    Q_OBJECT

    QVariant id;//идентификационный номер
    int code_all;//Общий номер
    int code_year;//Номер с начала года
    QDateTime date;
    QString person;//варщик
    int density;//плотность
    int juice;//сок
    QString type;//Тип пасты
    QTime start;//Начало варки
    QTime end;//Конец варки
    int dimensionFrom;//измерение, 1-е значение
    int dimensionTo;//измерение, 2-е значение
    float temperature;//Температура пасты при измерении
    QString description;//Описание


public:
    Data(QObject *parent = 0);
    Data(QObject *parent, QSqlQuery &qry);
    virtual ~Data(){}

    QVariant Id();
     bool isNew();

    int Code_all();
    void setCode_all(int value);

    int Code_year();
    void setCode_year(int value);

    QDateTime Date();
    QString Date_month();
    QString Date_day();
    void setDate(QDateTime value);

    QString Person();
    void setPerson(QString person);

    int Density();
    QString Density_plus();
    QString Density_display();
    void setDensity(int value);

    int Juice();
    void setJuice(int value);

    QString Type();
    void setType(QString type);

    QTime Start();
    void setStart(QTime value);

    QTime End();
    void setEnd(QTime value);

    int DimensionFrom();
    void setDimensionFrom(int value);

    int DimensionTo();
    void setDimensionTo(int value);

    float Temperature();
    void setTemperature(float value);

    QString Description();
    void setDescription(QString text);

    //Для поиска по тем параметрам, по которым поиск не ведётся
   QString Report();

};

#endif // DATA_H
