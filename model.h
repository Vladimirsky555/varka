#ifndef MODEL_H
#define MODEL_H

#include <QObject>
#include <QSqlTableModel>
#include <QTime>


#include "data.h"
#include "dialog.h"
#include "showitemwindow.h"

class Model : public QSqlTableModel
{

    Q_OBJECT

    QList<Data*>*items;//Список указателей на элементы
    QList<Data*>*s_items;

    QModelIndex currentIndex;
    QString pattern;//для подсветки поиска по комментарию

    QString report;//Текст отчёта о варке
    QStringList lst;//список варщиков

public:
    //Экшены
    QList<QAction*> allActions;
    QAction *actEditItem;
    QAction *actDeleteItem;
    QAction *actShowItem;

public:
    Model(QObject *parent = 0);
    virtual ~Model(){}

    void selectAll();
    int getCount();
    Data* getItemById(int id);
    QStringList defineLstPerson();

    bool checkReport(QString pattern, QString report);
    void search(QDate date, bool _date, int flag, QString person, bool _person,
                       QString type, bool _type, int density, bool _density,
                       QString pattern, bool _description);

    void addItem(Data *item);//В модель
    void addData();//В модель и базу

    void setItems();
    void clearItems();

protected slots:
    void edit_item();//В модель и базу
    void delete_item();
    void show_item();

private:
    delete_from_db(Data* item);
    bool checkRegExp(QRegExp rx);

public slots:
    void acceptIndexfromView(QModelIndex index);
    void acceptPattern(QString pattern);//для подстветки

private slots:
    bool save_to_db(Data *item);
    bool update_in_db(Data *item);
    void shutdown();

    //Вспомогательные функции
protected:
    virtual QVariant    dataDisplay(const QModelIndex &index) const;
    virtual QVariant    dataTextAlignment(const QModelIndex &) const;
    virtual QVariant    dataBackground(const QModelIndex &index) const;//Заливка ячейки
    virtual Data *getItem(const QModelIndex &index)const;
    virtual QVariant    dataToolTip(const QModelIndex &I) const;

    // QSqlTableModel interface
public:
    virtual int rowCount(const QModelIndex &parent) const;
    virtual int columnCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    virtual Qt::ItemFlags flags(const QModelIndex &) const;

signals:
    void shutdown_w();
};

#endif // MODEL_H
