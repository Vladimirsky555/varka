#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QFileDialog>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Таблица учёта варок");

    //Создаём модель
    M = new Model(this);
    M->setEditStrategy(QSqlTableModel::OnFieldChange);

    //Устанавливаем текущую дату в поисковике
    ui->dateEdit_m->setDate(QDate::currentDate());

    //Картинка на главный экран
    QPixmap pix(":/images/pix.jpg");
    ui->label_pix->setPixmap(pix.scaledToWidth(250));

    //Оформление кнопки для очистки перед новым поиском
    ui->btnClear->setDefaultAction(ui->actionClear);
    connect(ui->actionClear, SIGNAL(triggered()),
            this, SLOT(clearBoxes()));

    //Оформление кнопки для экспорта в JSON
    ui->btnExportToJSON->setDefaultAction(ui->actionExportToJSON);//привязали к toolbutton
    connect(ui->actionExportToJSON, SIGNAL(triggered()),this, SLOT(ExportToJSON()));
    QPixmap pExp(":/images/json.png");
    ui->actionExportToJSON->setIcon(QIcon(pExp));
    ui->btnExportToJSON->setEnabled(false);

    printList();//Выводим список варок в представление
    fillBoxes();//Заполняем комбобоксы и получаем список варщиков для поиска
    createUI();//Создаём представление
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createUI()
{
    ui->tableView->setModel(M);

    connect(this, SIGNAL(sendIndextoModel(QModelIndex)),
            M, SLOT(acceptIndexfromView(QModelIndex)));

    connect(this, SIGNAL(shutdown()),
            M, SLOT(shutdown()));

    connect(this, SIGNAL(sendPattern(QString)),
            M, SLOT(acceptPattern(QString)));

    //Добавляем события в представление
    ui->tableView->addActions(M->allActions);
    ui->tableView->setContextMenuPolicy(Qt::ActionsContextMenu);

    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);

    ui->tableView->horizontalHeader()->setSectionResizeMode(13, QHeaderView::Stretch);
    ui->tableView->resizeColumnToContents(2);
    ui->tableView->resizeColumnToContents(5);
    ui->tableView->resizeColumnToContents(6);
    ui->tableView->resizeColumnToContents(8);
    ui->tableView->resizeColumnToContents(9);
    ui->tableView->resizeColumnToContents(10);
    ui->tableView->resizeColumnToContents(11);
    ui->tableView->resizeColumnToContents(12);
	
    ui->tableView->setColumnHidden(0, true);
    ui->tableView->setColumnHidden(3, true);
    ui->tableView->setColumnHidden(7, true);
    ui->tableView->setAlternatingRowColors(false);
}

//Добавляем новую варку
void MainWindow::on_addVarka_clicked()
{
    M->addData();
}

void MainWindow::on_tableView_clicked(const QModelIndex &index)
{
    emit (sendIndextoModel(index));
}

void MainWindow::on_btnAll_clicked()
{
    printList();
}

void MainWindow::on_btnSearch_clicked()
{
   ui->addVarka->setEnabled(false);

    QDate date;
    QString person;
    QString type;
    QString description;
    int density;
    int flag;

    bool _date;
    bool _person;
    bool _type;
    bool _density;
    bool _description;

    M->clearItems();
    M->selectAll();//Заполняем массив items, иначе не будет результатов поиска

    date = ui->dateEdit_m->date();

    if(ui->rYear_m->isChecked()){
        flag = 1;
    } else if(ui->rMonth_m->isChecked()){
        flag = 2;
    } else if(ui->rDay_m->isChecked()){
        flag = 3;
    }

    person = lst.at(ui->cbxPerson_m->currentIndex());

    if(ui->cbxType_m->currentIndex() == 0){
        type = "---";
    } else if(ui->cbxType_m->currentIndex()  == 1){
        type = "Premium";
    }else if(ui->cbxType_m->currentIndex()  == 2){
        type = "Yes";
    }else if(ui->cbxType_m->currentIndex()  == 3){
        type = "Exclusive";
    }else if(ui->cbxType_m->currentIndex()  == 4){
        type = "Sugar";
    }

    if(ui->cbxDensity_m->currentIndex() == 0){
        density = 0;
    } else if(ui->cbxDensity_m->currentIndex() == 1){
        density = 100;
    }else if(ui->cbxDensity_m->currentIndex() == 2){
        density = 90;
    }else if(ui->cbxDensity_m->currentIndex() == 3){
        density = 80;
    }else if(ui->cbxDensity_m->currentIndex() == 4){
        density = 70;
    }else if(ui->cbxDensity_m->currentIndex() == 5){
        density = 60;
    }else if(ui->cbxDensity_m->currentIndex() == 6){
        density = 50;
    }

    description = ui->edtPattern_m->text();

    //Определяем какие параметры использовать
    if(ui->cbDate->isChecked()){
        _date = true;
    } else {
        _date = false;
    }

    if(ui->edtPattern_m->text() == ""){
        _description = false;
    } else {
        _description = true;
    }

    _person = check_index(ui->cbxPerson_m->currentIndex());
    _type = check_index(ui->cbxType_m->currentIndex());
    _density = check_index(ui->cbxDensity_m->currentIndex());

    M->search(date, _date, flag, person, _person, type, _type, density, _density,
              description, _description);
}

void MainWindow::fillBoxes()
{
    lst = M->defineLstPerson();//получаем список варщиков
    ui->cbxPerson_m->addItems(lst);

    ui->cbxDensity_m->addItem("-------");
    ui->cbxDensity_m->addItem("0 (bandage)");
    ui->cbxDensity_m->addItem("1 (ultrasoft)");
    ui->cbxDensity_m->addItem("2 (soft)");
    ui->cbxDensity_m->addItem("3 (medium)");
    ui->cbxDensity_m->addItem("4 (hard)");
    ui->cbxDensity_m->addItem("5 (extrahard)");

    ui->cbxType_m->addItem("---");
    ui->cbxType_m->addItem("Premium");
    ui->cbxType_m->addItem("Yes");
    ui->cbxType_m->addItem("Exclusive");
    ui->cbxType_m->addItem("Sugar");
}

bool MainWindow::check_index(int index)
{
    if(index == 0){
        return false;
    }

    return true;
}

//Вывод списка с варками в представление
void MainWindow::printList()
{
    ui->btnExportToJSON->setEnabled(true);
    ui->addVarka->setEnabled(true);
    M->selectAll();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    emit shutdown();
    QWidget::closeEvent(event);
}


void MainWindow::on_edtPattern_m_textChanged(const QString &str)
{
    emit sendPattern(str);//отправляем поисковое слово для подстветки
}

void MainWindow::clearBoxes()
{
    ui->cbxDensity_m->setCurrentIndex(0);
    ui->cbxPerson_m->setCurrentIndex(0);
    ui->cbxType_m->setCurrentIndex(0);
    ui->edtPattern_m->setText("");
    ui->rYear_m->setChecked(true);
    ui->cbDate->setChecked(false);
}

void MainWindow::ExportToJSON()
{
    QJsonArray textsArray = m_currentJsonObject["var"].toArray();
    QJsonObject textObject;

    for(int i = 0; i < M->getCount(); i++)
    {
        textObject["id"] = QString::number(i+1);
        textObject["code_all"] = QString::number(M->getItemById(i)->Code_all());
        textObject["code_year"] = QString::number(M->getItemById(i)->Code_year());
        textObject["date"] = M->getItemById(i)->Date().toString("dd.MM.yyyy");
        textObject["person"] = M->getItemById(i)->Person();
        textObject["density"] = M->getItemById(i)->Density();
        textObject["juice"] = M->getItemById(i)->Juice();
        textObject["type"] = M->getItemById(i)->Type();
        textObject["start"] = M->getItemById(i)->Start().toString("hh:mm");
        textObject["end"] = M->getItemById(i)->End().toString("hh:mm");
        textObject["dimensionFrom"] = M->getItemById(i)->DimensionFrom();
        textObject["dimensionTo"] = M->getItemById(i)->DimensionTo();
        textObject["temperature"] = M->getItemById(i)->Temperature();
        textObject["description"] = M->getItemById(i)->Description();

        textsArray.append(textObject);
    }

    m_currentJsonObject["var"] = textsArray;

    // С помощью диалогового окна получаем имя файла с абсолютным путём
       QString saveFileName = QFileDialog::getSaveFileName(this,
                                                           tr("Save Json File"),
                                                           QString(),
                                                           tr("JSON (*.json)"));
       QFileInfo fileInfo(saveFileName);   // С помощью QFileInfo
       QDir::setCurrent(fileInfo.path());  // установим текущую рабочую директорию, где будет файл, иначе может не заработать
       // Создаём объект файла и открываем его на запись
       QFile jsonFile(saveFileName);
       if (!jsonFile.open(QIODevice::WriteOnly))
       {
           return;
       }

       // Записываем текущий объект Json в файл
       //jsonFile.write(QJsonDocument(m_currentJsonObject).toJson(QJsonDocument::Indented));

       //Чтобы не править json-файл вручную перед каждой загрузкой на сервер
       jsonFile.write("[");
       for(int i = 0; i < textsArray.size(); i++)
       {
           jsonFile.write(QJsonDocument(textsArray.at(i).toObject()).toJson(QJsonDocument::Indented));

           if(i < textsArray.size()-1){
               jsonFile.write(",");
           }
       }
       jsonFile.write("]");

       jsonFile.close();   // Закрываем файл
}

