#include "model.h"

#include <QSqlQuery>
#include <QFont>
#include <QColor>
#include <QAction>
#include <QDebug>
#include <QSqlError>
#include <QMessageBox>
#include <QRegExp>


Model::Model(QObject *parent) : QSqlTableModel(parent)
{
    items = new QList<Data*>;
    s_items = new QList<Data*>;

    //Список экшенов
    {
        QAction *A = actShowItem = new QAction(tr("Просмотр"), this);
        QPixmap p(":/images/eye.png"); A->setIcon(QIcon(p));
        A->setShortcut(tr("Ctrl+S"));
        connect(A, SIGNAL(triggered()), this, SLOT(show_item()));
        allActions << A;
    }{
        QAction *A = actEditItem = new QAction(tr("Редактировать"), this);
        QPixmap p(":/images/edit.png"); A->setIcon(QIcon(p));
        A->setShortcut(tr("Ctrl+E"));
        connect(A, SIGNAL(triggered()), this, SLOT(edit_item()));
        allActions << A;
    }{
        QAction *A = actDeleteItem = new QAction(tr("Удалить"), this);
        QPixmap p(":/images/delete1.png"); A->setIcon(QIcon(p));
        A->setShortcut(tr("Ctrl+D"));
        connect(A, SIGNAL(triggered()), this, SLOT(delete_item()));
        allActions << A;
    }
}


void Model::selectAll()
{
    items->clear();
    QSqlQuery qry;
    qry.prepare(
                "SELECT                   \n"
                "   id,                        \n"
                "   code_all,             \n"
                "   code_year,           \n"
                "   date,                  \n"
                "   person,                  \n"
                "   density,                  \n"
                "   juice,                 \n"
                "   type,                 \n"
                "   start,                  \n"
                "   end,                  \n"
                "   dimFrom,                  \n"
                "   dimTo,                  \n"
                "   temperature,                  \n"
                "   description                  \n"
                "   FROM varkaDB          \n"
                "   ORDER BY code_all DESC;   \n"
                );

    if(qry.exec()){
        while(qry.next()){
            Data *item = new Data(this, qry);
            addItem(item);
        }
    } else {
        qCritical() << "Cannot execute query!";
        QSqlError err = qry.lastError();
        qCritical() << err.nativeErrorCode();
        qCritical() << err.databaseText().toUtf8().data();
        qCritical() << err.driverText().toUtf8().data();
        qDebug() << qry.executedQuery();
    }

//    setQuery(qry);
    beginResetModel();
    endResetModel();
}

int Model::getCount()
{
    return items->count();
}

Data *Model::getItemById(int id)
{
    return items->at(id);
}


bool Model::checkReport(QString pattern, QString report)
{
    QRegExp rx(pattern);
    if(!checkRegExp(rx))return false;
    int pos = 0;
    while((pos = rx.indexIn(report, pos)) != -1){
        pos += rx.matchedLength();
        return true;
    }

    return false;
}

void Model::addItem(Data *item)
{
    if(item != NULL){
        items->append(item);
    }
    return;
}

void Model::addData()
{
    Data *item = new Data();
    item->setCode_all(items->at(0)->Code_all() + 1);//Выставляю следующий номер
    item->setCode_year(items->at(0)->Code_year() + 1);

    Dialog d(item, true);
    d.save();
    d.exec();

    //Защита от добавления варки, если не обозначен варщик
    //Добавиться может только варка, где есть конкретный варщик, а не
    //абстрактный
    if(item->Person() == "---"){
        delete item;
        return;
    }

    beginResetModel();
    endResetModel();

    save_to_db(item);//В базу данных
    addItem(item);//В модель
}

void Model::setItems()
{
    this->items->clear();
    for(int i = 0; i < s_items->count(); i++){
        this->items->append(s_items->at(i));
    }
}

void Model::clearItems()
{
    items->clear();
}

void Model::edit_item()
{  
    Data *item = getItem(currentIndex);
    if(!item)return;

    Dialog d(item, false);
    d.save();
    d.exec();
    items->replace(currentIndex.row(), item);
    update_in_db(item);

    beginResetModel();
    endResetModel();
}

void Model::delete_item()
{
    Data *item = getItem(currentIndex);
    if(!item)return;

    int n = QMessageBox::warning(0, "Предупреждение", "Неужели удалить эту варку ?",
                                 "Да", "Нет", QString(), 0, 1);
    if(!n) {
        beginRemoveRows(QModelIndex(), currentIndex.row(), currentIndex.row());
        items->removeOne(item);
        delete item;
        endRemoveRows();

        delete_from_db(item);//Из базы также удаляем

        items->clear();
        selectAll();
    }
}

void Model::show_item()
{
    Data* item = getItem(currentIndex);

    if(!item){
        QMessageBox::information(NULL, "Сообщение", "Вы не выбрали ни одного элемента. "
                                       "Для выбора элемента кликните левой кнопокой мыши по нему.");
        return;
    }

    ShowItemWindow *show = new ShowItemWindow(item, pattern, item->Description());

    connect(this, SIGNAL(shutdown_w()),
            show, SLOT(shutdown()));

    show->show();
}

Model::delete_from_db(Data *item)
{
    QSqlQuery query;
    query.setForwardOnly(true);
    query.prepare("DELETE FROM varkaDB WHERE id = :ID ;");
    query.bindValue(":ID", item->Id());

    if(!query.exec()){
        qCritical() << query.lastError().databaseText().toUtf8().data();
        qCritical() << query.lastError().driverText();
        qCritical() << query.lastError().nativeErrorCode();
        return false;
    } else {
        return true;
    }
    return false;
}

bool Model::checkRegExp(QRegExp rx)
{
    if(rx.isValid() && !rx.isEmpty() && !rx.exactMatch("")){
        return true;
    } else {
        QMessageBox::information(NULL,"Информсообщение",
                                 trUtf8("Некорректный шаблон регулярного выражения!"));
        return false;
    }
}

void Model::acceptIndexfromView(QModelIndex index)
{
    this->currentIndex = index;
}

void Model::acceptPattern(QString pattern)
{
    this->pattern = pattern;
}


bool Model::save_to_db(Data *item)
{   
    QSqlQuery query;
    query.setForwardOnly(true);

    query.prepare("INSERT INTO varkaDB (code_all, code_year, date, person, density, juice, type, start, end,"
                  "dimFrom, dimTo, temperature, description)"
                  "VALUES (:code_all, :code_year, :date, :person, :density, :juice, :type, :start, :end,"
                  ":dimFrom, :dimTo, :temperature, :description)");

    query.bindValue(":code_all", item->Code_all());
    query.bindValue(":code_year", item->Code_year());
    query.bindValue(":date", item->Date());
    query.bindValue(":person", item->Person());
    query.bindValue(":density", item->Density());
    query.bindValue(":juice", item->Juice());
    query.bindValue(":type", item->Type());
    query.bindValue(":start", item->Start());
    query.bindValue(":end", item->End());
    query.bindValue(":dimFrom", item->DimensionFrom());
    query.bindValue(":dimTo", item->DimensionTo());
    query.bindValue(":temperature", item->Temperature());
    query.bindValue(":description", item->Description());

    if(query.exec()) return true;

    qCritical() << query.lastError().databaseText();
    qCritical() << query.lastError().driverText();
    qCritical() << query.lastError().nativeErrorCode();

    return false;
}

bool Model::update_in_db(Data *item)
{
    QSqlQuery query;
    query.setForwardOnly(true);
    query.prepare("UPDATE varkaDB SET  "
                  " id = :id, "
                  " code_all = :code_all, "
                  " code_year = :code_year, "
                  " date = :date, "
                  " person = :person, "
                  " density = :density, "
                  " juice = :juice, "
                  " type = :type,"
                  " start = :start, "
                  " end = :end, "
                  " dimFrom = :dimFrom, "
                  " dimTo = :dimTo, "
                  " temperature = :temperature, "
                  " description = :description "
                  "WHERE id = :id; "
                  );

    query.bindValue(":id", item->Id());
    query.bindValue(":code_all", item->Code_all());
    query.bindValue(":code_year", item->Code_year());
    query.bindValue(":date", item->Date());
    query.bindValue(":person", item->Person());
    query.bindValue(":density", item->Density());
    query.bindValue(":juice", item->Juice());
    query.bindValue(":type", item->Type());
    query.bindValue(":start", item->Start());
    query.bindValue(":end", item->End());
    query.bindValue(":dimFrom", item->DimensionFrom());
    query.bindValue(":dimTo", item->DimensionTo());
    query.bindValue(":temperature", item->Temperature());
    query.bindValue(":description", item->Description());

    if(query.exec()) {
        return true;
    } else {
        qCritical() << query.lastError().databaseText();
        qCritical() << query.lastError().driverText();
        qCritical() << query.lastError().nativeErrorCode();

        return false;
    }
}

void Model::shutdown()
{
    emit shutdown_w();
}

QVariant Model::dataDisplay(const QModelIndex &index) const
{
    Data *item = items->at(index.row());
    switch (index.column()) {
    case 0: return item->Id();
    case 1: return item->Date().isValid() ? item->Date().toString("dd.MM.yyyy") : "";
    case 2: return item->Code_all();
    case 3: return item->Code_year();
    case 4: return item->Person();
    case 5: return item->Density_display();
    case 6: return item->Type() != "Sugar" ? item->Density() : item->Density() + 15;
    case 7: return item->Juice();
    case 8: return item->Type();
    case 9: return item->Start().isValid() ? item->Start().toString("hh.mm") : "";
    case 10: return item->End().isValid() ? item->End().toString("hh.mm") : "";
    case 11: return item->DimensionFrom();
    case 12: return item->DimensionTo();
    case 13: return item->Temperature();
    case 14: return item->Description();
    default: return QVariant();
    }
}

QVariant Model::dataTextAlignment(const QModelIndex &) const
{
//    int Result = Qt::AlignBaseline;
    int Result = Qt::AlignVCenter;
//    Result = index.column() == 1 ? Qt::AlignLeft : Qt::AlignHCenter;
    return Result;
}

QVariant Model::dataBackground(const QModelIndex &index) const
{
    Data *item = items->at(index.row());
    QColor result;

    if(!item->isNew()) {
        if(item->Type() == "Premium"){
            result = QColor(240, 255, 255);
        } else if(item->Type() == "Yes" ){
            result = QColor(144, 238, 144);
        } else if(item->Type() == "Exclusive" ){
            result = QColor(32, 178, 170);
        } else if(item->Type() == "Sugar" ){
            result = QColor(255, 255, 0);
        } else {
            result = QColor("white");
        }
    } else {
        result = QColor("white");
    }

    result.setAlphaF(0.2);
    return result;
}

//QVariant Model::dataForeground(const QModelIndex &I) const
//{
//    Data *item = getItem(I);
//    if(!item) return QVariant();

////    QColor Result = item->getLocal() ? QColor("blue") : QColor("black");
//    QColor Result = QColor("black");
//    //if(!item->isActive()) Result.setAlphaF(0.50);//неактивный побледнеет
//    return Result;
//}

//QVariant Model::dataFont(const QModelIndex &I) const
//{
//   Data *item = getItem(I);
//    if(!item) return QVariant();

//    QFont F;
//    if(item->getDeleted()) F.setStrikeOut(true);
//        F.setItalic(true);

//    if(D->getLocal()){
//        F.setBold(true);
//    }

//    return F;
//}


Data *Model::getItem(const QModelIndex &index) const
{
    int id = index.row();
    if(id < 0 || id >= items->size()) return 0;
    return items->at(id);
}


QVariant Model::dataToolTip(const QModelIndex &I) const
{
    Data *item = getItem(I);
    if(!item) return QVariant();

    switch (I.column()) {
    case 1:
    {
        if(!item->Date().isValid())return QVariant();
        return tr("%1").arg(item->Date().toString("dd.MM.yyyy"));
    }
    case 13: return item->Description();
    default: return QVariant();
    }
}


int Model::rowCount(const QModelIndex &parent) const
{
    //Если родитель существует, то кол-во строк 0
    if(!parent.isValid()){
        return items->count();
    } else {
        return 0;
    }
}

int Model::columnCount(const QModelIndex &parent) const
{
    if(!parent.isValid()){
        return 15;
    } else {
        return 0;
    }
}

QVariant Model::data(const QModelIndex &index, int role) const
{
    switch (role) {
    case Qt::DisplayRole:             return dataDisplay(index);
    case Qt::TextAlignmentRole: return dataTextAlignment(index);
    case Qt::BackgroundRole:    return dataBackground(index);
    case Qt::ToolTipRole:             return dataToolTip(index);
        //    case Qt::ForegroundRole:      return dataForeground(index);
        //    case Qt::FontRole:                  return dataFont(index);

    default:  return QVariant();
    }
}

QVariant Model::headerData(int section, Qt::Orientation orientation, int role) const
{
    //Для горизонтальных заголовков оставляем данные по умолчанию
    if(orientation != Qt::Horizontal){
        return QAbstractTableModel::headerData(section,orientation,role);
    }

    switch (role) {
    case Qt::DisplayRole:
        switch (section) {
        case 0: return tr("Индекс");
        case 1: return tr("Дата");
        case 2: return tr("Номер");
        case 3: return tr("Номер2");
        case 4: return tr("Варщик");
        case 5: return tr("Плотность 1");
        case 6: return tr("Плотность 2");
        case 7: return tr("Сок");
        case 8: return tr("Тип пасты");
        case 9: return tr("Начало");
        case 10: return tr("Конец");
        case 11: return tr("От");
        case 12: return tr("До");
        case 13: return tr("Температура");
        case 14: return tr("Комментарии");
        default: return QVariant();
        }
    case Qt::TextAlignmentRole:
        return QVariant(Qt::AlignBaseline | Qt::AlignHCenter);
    case Qt::ForegroundRole:
    {
        QFont F;
        F.setBold(true);
        return F;
    }
    default: return QVariant();
    }
}

Qt::ItemFlags Model::flags(const QModelIndex &) const
{
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}


//Глобальный поиск по всем элементам
void Model::search(QDate date, bool _date, int flag, QString person, bool _person,
                          QString type, bool _type, int density, bool _density,
                          QString pattern, bool _description){

    s_items->clear();
    Data *item = new Data();

    //Ищем по 1) Дата 2) Варщик 3) Тип пасты 4) Плотность 5) Комментарий
    if(_date && _person && _type && _density && _description){
        if(flag == 1){
            for(int i = 0; i < items->count(); i++){
                Data *item = items->at(i);
                if(item->Date().toString("dd.MM.yyyy").right(4) == date.toString("dd.MM.yyyy").right(4)){
                    if(item->Person() == person){
                        if(item->Type() == type){
                            if(item->Density() == density){
                                if(checkReport(pattern, item->Report())){
                                    s_items->append(item);
                            }
                        }
                        }
                    }
                }
            }
        } else if(flag == 2){
            for(int i = 0; i < items->count(); i++){
                item = items->at(i);
                if(item->Date().toString("dd.MM.yyyy").right(7) == date.toString("dd.MM.yyyy").right(7)){
                    if(item->Person() == person){
                        if(item->Type() == type){
                            if(item->Density() == density){
                                if(checkReport(pattern, item->Report())){
                                    s_items->append(item);
                                }
                            }
                        }
                    }
                }
            }
        } else if(flag == 3){
            for(int i = 0; i < items->count(); i++){
                item = items->at(i);
                if(item->Date().date().toString("dd.MM.yyyy") == date.toString("dd.MM.yyyy")){
                    if(item->Person() == person){
                        if(item->Type() == type){
                            if(item->Density() == density){
                                if(checkReport(pattern, item->Report())){
                                    s_items->append(item);
                                }
                            }
                        }
                    }
                }
            }
        }//Ищем по 1) Дата 2) Варщик 3) Тип пасты 4) Плотность
    } else if(_date && _person && _type && _density && !_description) {
        if(flag == 1){
            for(int i = 0; i < items->count(); i++){
                Data *item = items->at(i);
                if(item->Date().toString("dd.MM.yyyy").right(4) == date.toString("dd.MM.yyyy").right(4)){
                    if(item->Person() == person){
                        if(item->Type() == type){
                            if(item->Density() == density){
                                s_items->append(item);
                            }
                        }
                    }
                }
            }
        } else if(flag == 2){
            for(int i = 0; i < items->count(); i++){
                item = items->at(i);
                if(item->Date().toString("dd.MM.yyyy").right(7) == date.toString("dd.MM.yyyy").right(7)){
                    if(item->Person() == person){
                        if(item->Type() == type){
                            if(item->Density() == density){
                                s_items->append(item);
                            }
                        }
                    }
                }
            }
        } else if(flag == 3){
            for(int i = 0; i < items->count(); i++){
                item = items->at(i);
                if(item->Date().date().toString("dd.MM.yyyy") == date.toString("dd.MM.yyyy")){
                    if(item->Person() == person){
                        if(item->Type() == type){
                            if(item->Density() == density){
                                s_items->append(item);
                            }
                        }
                    }
                }
            }
        }//1) Дата 2) Варщик 3) Тип пасты
    } else if(_date && _person && _type && !_density && !_description){
        if(flag == 1){
            for(int i = 0; i < items->count(); i++){
                Data *item = items->at(i);
                if(item->Date().toString("dd.MM.yyyy").right(4) == date.toString("dd.MM.yyyy").right(4)){
                    if(item->Person() == person){
                        if(item->Type() == type){
                            s_items->append(item);
                        }
                    }
                }
            }
        } else if(flag == 2){
            for(int i = 0; i < items->count(); i++){
                item = items->at(i);
                if(item->Date().toString("dd.MM.yyyy").right(7) == date.toString("dd.MM.yyyy").right(7)){
                    if(item->Person() == person){
                        if(item->Type() == type){
                            s_items->append(item);
                        }
                    }
                }
            }
        } else if(flag == 3){
            for(int i = 0; i < items->count(); i++){
                item = items->at(i);
                if(item->Date().date().toString("dd.MM.yyyy") == date.toString("dd.MM.yyyy")){
                    if(item->Person() == person){
                        if(item->Type() == type){
                            s_items->append(item);
                        }
                    }
                }
            }
        }//1) Дата 2) Варщик
    } else if(_date && _person && !_type && !_density && !_description) {
        if(flag == 1){
            for(int i = 0; i < items->count(); i++){
                Data *item = items->at(i);
                if(item->Date().toString("dd.MM.yyyy").right(4) == date.toString("dd.MM.yyyy").right(4)){
                    if(item->Person() == person){
                        s_items->append(item);
                    }
                }
            }
        } else if(flag == 2){
            for(int i = 0; i < items->count(); i++){
                item = items->at(i);
                if(item->Date().toString("dd.MM.yyyy").right(7) == date.toString("dd.MM.yyyy").right(7)){
                    if(item->Person() == person){
                        s_items->append(item);
                    }
                }
            }
        } else if(flag == 3){
            for(int i = 0; i < items->count(); i++){
                item = items->at(i);
                if(item->Date().date().toString("dd.MM.yyyy") == date.toString("dd.MM.yyyy")){
                    if(item->Person() == person){
                        s_items->append(item);
                    }
                }
            }
        }//1) Дата
    } else if(_date && !_person && !_type && !_density && !_description){
        if(flag == 1){
            for(int i = 0; i < items->count(); i++){
                Data *item = items->at(i);
                if(item->Date().toString("dd.MM.yyyy").right(4) == date.toString("dd.MM.yyyy").right(4)){
                    s_items->append(item);
                }
            }
        } else if(flag == 2){
            for(int i = 0; i < items->count(); i++){
                item = items->at(i);
                if(item->Date().toString("dd.MM.yyyy").right(7) == date.toString("dd.MM.yyyy").right(7)){
                    s_items->append(item);
                }
            }
        } else if(flag == 3){
            for(int i = 0; i < items->count(); i++){
                item = items->at(i);
                if(item->Date().date().toString("dd.MM.yyyy") == date.toString("dd.MM.yyyy")){
                    s_items->append(item);
                }
            }
        }//1) Варщик 2) Тип пасты 3) Плотность 4) Комментарий
    } else if(!_date && _person && _type && _density && _description){
        for(int i = 0; i < items->count(); i++){
            Data *item = items->at(i);
            if(item->Person() == person){
                if(item->Type() == type){
                    if(item->Density() == density){
                        if(checkReport(pattern, item->Report())){
                            s_items->append(item);
                        }
                    }
                }
            }
        }//Ищем по 2) Варщик 3) Тип пасты 4) Плотность
    } else if(!_date && _person && _type && _density && !_description){
        for(int i = 0; i < items->count(); i++){
            Data *item = items->at(i);
            if(item->Person() == person){
                if(item->Type() == type){
                    if(item->Density() == density){
                        s_items->append(item);
                    }
                }
            }
        }//Ищем по 2) Варщик 3) Тип пасты
    } else if(!_date && _person && _type && !_density && !_description){
        for(int i = 0; i < items->count(); i++){
            Data *item = items->at(i);
            if(item->Person() == person){
                if(item->Type() == type){
                    s_items->append(item);
                }
            }
        }//Ищем по 2) Варщик
    } else if(!_date && _person && !_type && !_density && !_description){
        for(int i = 0; i < items->count(); i++){
            Data *item = items->at(i);
            if(item->Person() == person){
                s_items->append(item);
            }
        }//3) Тип пасты 4) Плотность 5) Комментарий
    } else if(!_date && !_person && _type && _density && _description){
        for(int i = 0; i < items->count(); i++){
            Data *item = items->at(i);
            if(item->Type() == type){
                if(item->Density() == density){
                    if(checkReport(pattern, item->Report())){
                        s_items->append(item);
                    }
                }
            }
        }//Ищем по 3) Тип пасты 4) Плотность
    } else if(!_date && !_person && _type && _density && !_description){
        for(int i = 0; i < items->count(); i++){
            Data *item = items->at(i);
            if(item->Type() == type){
                if(item->Density() == density){
                    s_items->append(item);
                }
            }
        }//Ищем по 3) Тип пасты
    } else if(!_date && !_person && _type && !_density && !_description){
        for(int i = 0; i < items->count(); i++){
            Data *item = items->at(i);
            if(item->Type() == type){
                s_items->append(item);
            }
        }//Ищем по 4) Плотность 5) Комментарий
    } else if(!_date && !_person && !_type && _density && _description){
        for(int i = 0; i < items->count(); i++){
            Data *item = items->at(i);
            if(item->Density() == density){
                if(checkReport(pattern, item->Report())){
                    s_items->append(item);
                }
            }
        }//Ищем по 4) Плотность
    } else if(!_date && !_person && !_type && _density && !_description){
        for(int i = 0; i < items->count(); i++){
            Data *item = items->at(i);
            if(item->Density() == density){
                s_items->append(item);
            }
        }//Ищем по 5) Комментарий
    } else if(!_date && !_person && !_type && !_density && _description){
        for(int i = 0; i < items->count(); i++){
            Data *item = items->at(i);
            if(checkReport(pattern, item->Report())){
                s_items->append(item);
            }
        }//Ищем по 1) Дата 2) Варщик 4) Плотность 5) Комментарий
    } else if(_date && _person && !_type && _density && _description){
        if(flag == 1){
            for(int i = 0; i < items->count(); i++){
                Data *item = items->at(i);
                if(item->Date().toString("dd.MM.yyyy").right(4) == date.toString("dd.MM.yyyy").right(4)){
                    if(item->Person() == person){
                        if(item->Density() == density){
                            if(checkReport(pattern, item->Report())){
                                s_items->append(item);
                            }
                        }
                    }
                }
            }
        } else if(flag == 2){
            for(int i = 0; i < items->count(); i++){
                item = items->at(i);
                if(item->Date().toString("dd.MM.yyyy").right(7) == date.toString("dd.MM.yyyy").right(7)){
                    if(item->Person() == person){
                        if(item->Density() == density){
                            if(checkReport(pattern, item->Report())){
                                s_items->append(item);
                            }
                        }
                    }
                }
            }
        } else if(flag == 3){
            for(int i = 0; i < items->count(); i++){
                item = items->at(i);
                if(item->Date().date().toString("dd.MM.yyyy") == date.toString("dd.MM.yyyy")){
                    if(item->Person() == person){
                        if(item->Density() == density){
                            if(checkReport(pattern, item->Report())){
                                s_items->append(item);
                            }
                        }
                    }
                }
            }
        }//1) Дата 2) Плотность 3) Комментарий
    } else if(_date && !_person && !_type && _density && _description){
        if(flag == 1){
            for(int i = 0; i < items->count(); i++){
                Data *item = items->at(i);
                if(item->Date().toString("dd.MM.yyyy").right(4) == date.toString("dd.MM.yyyy").right(4)){
                    if(item->Density() == density){
                        if(checkReport(pattern, item->Report())){
                            s_items->append(item);
                        }
                    }
                }
            }
        } else if(flag == 2){
            for(int i = 0; i < items->count(); i++){
                item = items->at(i);
                if(item->Date().toString("dd.MM.yyyy").right(7) == date.toString("dd.MM.yyyy").right(7)){
                    if(item->Density() == density){
                        if(checkReport(pattern, item->Report())){
                            s_items->append(item);
                        }
                    }
                }
            }
        } else if(flag == 3){
            for(int i = 0; i < items->count(); i++){
                item = items->at(i);
                if(item->Date().date().toString("dd.MM.yyyy") == date.toString("dd.MM.yyyy")){
                    if(item->Density() == density){
                        if(checkReport(pattern, item->Report())){
                            s_items->append(item);
                        }
                    }
                }
            }
        }//1) Дата 3) Комментарий
    } else if(_date && !_person && !_type && !_density && _description){
        if(flag == 1){
            for(int i = 0; i < items->count(); i++){
                Data *item = items->at(i);
                if(item->Date().toString("dd.MM.yyyy").right(4) == date.toString("dd.MM.yyyy").right(4)){
                    if(checkReport(pattern, item->Report())){
                        s_items->append(item);
                    }
                }
            }
        } else if(flag == 2){
            for(int i = 0; i < items->count(); i++){
                item = items->at(i);
                if(item->Date().toString("dd.MM.yyyy").right(7) == date.toString("dd.MM.yyyy").right(7)){
                    if(checkReport(pattern, item->Report())){
                        s_items->append(item);
                    }
                }
            }
        } else if(flag == 3){
            for(int i = 0; i < items->count(); i++){
                item = items->at(i);
                if(item->Date().date().toString("dd.MM.yyyy") == date.toString("dd.MM.yyyy")){
                    if(checkReport(pattern, item->Report())){
                        s_items->append(item);
                    }
                }
            }
        }//1) Дата 2) Тип пасты 3) Плотность 4) Комментарий
    } else if(_date && !_person && _type && _density && _description){
        if(flag == 1){
            for(int i = 0; i < items->count(); i++){
                Data *item = items->at(i);
                if(item->Date().toString("dd.MM.yyyy").right(4) == date.toString("dd.MM.yyyy").right(4)){
                    if(item->Type() == type){
                        if(item->Density() == density){
                            if(checkReport(pattern, item->Report())){
                                s_items->append(item);
                            }
                        }
                    }
                }
            }
        } else if(flag == 2){
            for(int i = 0; i < items->count(); i++){
                item = items->at(i);
                if(item->Date().toString("dd.MM.yyyy").right(7) == date.toString("dd.MM.yyyy").right(7)){
                    if(item->Type() == type){
                        if(item->Density() == density){
                            if(checkReport(pattern, item->Report())){
                                s_items->append(item);
                            }
                        }
                    }
                }
            }
        } else if(flag == 3){
            for(int i = 0; i < items->count(); i++){
                item = items->at(i);
                if(item->Date().date().toString("dd.MM.yyyy") == date.toString("dd.MM.yyyy")){
                    if(item->Type() == type){
                        if(item->Density() == density){
                            if(checkReport(pattern, item->Report())){
                                s_items->append(item);
                            }
                        }
                    }
                }
            }
        }//Ищем 1) Тип пасты 2) Комментарий
    } else if(!_date && !_person && _type && !_density && _description){
        for(int i = 0; i < items->count(); i++){
            Data *item = items->at(i);
            if(item->Type() == type){
                if(checkReport(pattern, item->Report())){
                    s_items->append(item);
                }
            }
        }//Ищем 1) Варщик 2) Плотность
    } else if(!_date && _person && !_type && _density && !_description){
        for(int i = 0; i < items->count(); i++){
            Data *item = items->at(i);
            if(item->Person() == person){
                if(item->Density() == density){
                    s_items->append(item);
                }
            }
        } // 1) Дата 2) Плотность
    } else if(_date && !_person && !_type && _density && !_description){
        if(flag == 1){
            for(int i = 0; i < items->count(); i++){
                Data *item = items->at(i);
                if(item->Date().toString("dd.MM.yyyy").right(4) == date.toString("dd.MM.yyyy").right(4)){
                    if(item->Density() == density){
                        s_items->append(item);
                    }
                }
            }
        } else if(flag == 2){
            for(int i = 0; i < items->count(); i++){
                item = items->at(i);
                if(item->Date().toString("dd.MM.yyyy").right(7) == date.toString("dd.MM.yyyy").right(7)){
                    if(item->Density() == density){
                        s_items->append(item);
                    }
                }
            }
        } else if(flag == 3){
            for(int i = 0; i < items->count(); i++){
                item = items->at(i);
                if(item->Date().date().toString("dd.MM.yyyy") == date.toString("dd.MM.yyyy")){
                    if(item->Density() == density){
                        s_items->append(item);
                    }
                }
            }
        }//Ищем 1) Дату 2) Тип пасты
    } else if(_date && !_person && _type && !_density && !_description){
        if(flag == 1){
            for(int i = 0; i < items->count(); i++){
                Data *item = items->at(i);
                if(item->Date().toString("dd.MM.yyyy").right(4) == date.toString("dd.MM.yyyy").right(4)){
                    if(item->Type() == type){
                        s_items->append(item);
                    }
                }
            }
        } else if(flag == 2){
            for(int i = 0; i < items->count(); i++){
                item = items->at(i);
                if(item->Date().toString("dd.MM.yyyy").right(7) == date.toString("dd.MM.yyyy").right(7)){
                    if(item->Type() == type){
                        s_items->append(item);
                    }
                }
            }
        } else if(flag == 3){
            for(int i = 0; i < items->count(); i++){
                item = items->at(i);
                if(item->Date().date().toString("dd.MM.yyyy") == date.toString("dd.MM.yyyy")){
                    if(item->Type() == type){
                        s_items->append(item);
                    }
                }
            }
        }//Ищем  1) Варщик 2) Плотность 3) Комментарий
    } else if(!_date && _person && !_type && _density && _description){
        for(int i = 0; i < items->count(); i++){
            Data *item = items->at(i);
            if(item->Person() == person){
                if(item->Density() == density){
                    if(checkReport(pattern, item->Report())){
                        s_items->append(item);
                    }
                }
            }
        }//Ищем по 1) Варщик 2) Тип пасты 3) Комментарий
    } else if(!_date && _person && _type && !_density && _description){
        for(int i = 0; i < items->count(); i++){
            Data *item = items->at(i);
            if(item->Person() == person){
                if(item->Type() == type){
                    if(checkReport(pattern, item->Report())){
                        s_items->append(item);
                    }
                }
            }
        }//1) Варщик 2) Комментарий
    } else if(!_date && _person && !_type && !_density && _description){
        for(int i = 0; i < items->count(); i++){
            Data *item = items->at(i);
            if(item->Person() == person){
                if(checkReport(pattern, item->Report())){
                    s_items->append(item);
                }
            }
        }//1) Дата 2) Варщик 3) Комментарий
    } else if(_date && _person && !_type && !_density && _description){
        if(flag == 1){
            for(int i = 0; i < items->count(); i++){
                Data *item = items->at(i);
                if(item->Date().toString("dd.MM.yyyy").right(4) == date.toString("dd.MM.yyyy").right(4)){
                    if(item->Person() == person){
                        if(checkReport(pattern, item->Report())){
                            s_items->append(item);
                        }
                    }
                }
            }
        } else if(flag == 2){
            for(int i = 0; i < items->count(); i++){
                item = items->at(i);
                if(item->Date().toString("dd.MM.yyyy").right(7) == date.toString("dd.MM.yyyy").right(7)){
                    if(item->Person() == person){
                        if(checkReport(pattern, item->Report())){
                            s_items->append(item);
                        }
                    }
                }
            }
        } else if(flag == 3){
            for(int i = 0; i < items->count(); i++){
                item = items->at(i);
                if(item->Date().date().toString("dd.MM.yyyy") == date.toString("dd.MM.yyyy")){
                    if(item->Person() == person){
                        if(checkReport(pattern, item->Report())){
                            s_items->append(item);
                        }
                    }
                }
            }
        }//1) Дата 2) Варщик 3) Плотность
    } else if(_date && _person && !_type && _density && !_description) {
        if(flag == 1){
            for(int i = 0; i < items->count(); i++){
                Data *item = items->at(i);
                if(item->Date().toString("dd.MM.yyyy").right(4) == date.toString("dd.MM.yyyy").right(4)){
                    if(item->Person() == person){
                            if(item->Density() == density){
                                s_items->append(item);
                            }
                    }
                }
            }
        } else if(flag == 2){
            for(int i = 0; i < items->count(); i++){
                item = items->at(i);
                if(item->Date().toString("dd.MM.yyyy").right(7) == date.toString("dd.MM.yyyy").right(7)){
                    if(item->Person() == person){
                            if(item->Density() == density){
                                s_items->append(item);
                            }
                    }
                }
            }
        } else if(flag == 3){
            for(int i = 0; i < items->count(); i++){
                item = items->at(i);
                if(item->Date().date().toString("dd.MM.yyyy") == date.toString("dd.MM.yyyy")){
                    if(item->Person() == person){
                            if(item->Density() == density){
                                s_items->append(item);
                            }
                        }
                }
            }
        }//1) Дата 2) Тип 3) Плотность
    } else if(_date && !_person && _type && _density && !_description){
        if(flag == 1){
            for(int i = 0; i < items->count(); i++){
                Data *item = items->at(i);
                if(item->Date().toString("dd.MM.yyyy").right(4) == date.toString("dd.MM.yyyy").right(4)){
                        if(item->Type() == type){
                            if(item->Density() == density){
                                s_items->append(item);
                            }
                    }
                }
            }
        } else if(flag == 2){
            for(int i = 0; i < items->count(); i++){
                item = items->at(i);
                if(item->Date().toString("dd.MM.yyyy").right(7) == date.toString("dd.MM.yyyy").right(7)){
                        if(item->Type() == type){
                            if(item->Density() == density){
                                s_items->append(item);
                            }
                        }
                }
            }
        } else if(flag == 3){
            for(int i = 0; i < items->count(); i++){
                item = items->at(i);
                if(item->Date().date().toString("dd.MM.yyyy") == date.toString("dd.MM.yyyy")){
                        if(item->Type() == type){
                            if(item->Density() == density){
                                s_items->append(item);
                            }
                        }
                }
            }
        }//Что-то ещё
    } else {
         qDebug() << "Нет параметра для поиска";
    }

    if(s_items->count() == 0){
        items->clear();
        return;
    }

    beginResetModel();
    setItems();
    endResetModel();
}

