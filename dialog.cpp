#include "dialog.h"
#include "ui_dialog.h"

#include <QIntValidator>


Dialog::Dialog(Data *item, QStringList lst, bool edit, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    this->item = item;
    this->edit = edit;
    this->lst = lst;

    fillBoxes();

    ui->edtCode_all->setValidator(new QIntValidator(0, 10000));
    ui->edtCode_year->setValidator(new QIntValidator(0, 10000));
    ui->edtDimensionFrom->setValidator(new QIntValidator(0, 100));
    ui->edtDimensionTo->setValidator(new QIntValidator(0, 100));
    ui->edtJuice->setValidator(new QIntValidator(0, 10000));

    if(this->edit){
        setWindowTitle("Добавление варки");
        ui->cbxPerson->setEditable(true);
        ui->dateEdit->setDate(QDate::currentDate());
        ui->edtCode_all->setText(QString::number(item->Code_all()));
        ui->edtCode_year->setText(QString::number(item->Code_year()));
        ui->edtJuice->setText(QString::number(400));
        ui->edtDimensionFrom->setText(QString::number(0));
        ui->edtDimensionTo->setText(QString::number(0));
        ui->edtTemp->setText(QString::number(0));
        ui->textEdit->setText("вода - \n");
    } else{
        load();
        setWindowTitle("Редактирование варки");
        ui->btnAdd->setText("Редактировать");
    }
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::load()
{    
    ui->edtCode_all->setText(QString::number(item->Code_all()));
    ui->edtCode_year->setText(QString::number(item->Code_year()));
    ui->dateEdit->setDateTime(item->Date());

    ui->cbxPerson->setCurrentIndex(personId());

    if(item->Density() == 0){
        ui->cbxDensity->setCurrentIndex(0);
    } else if(item->Density() == 100){
        ui->cbxDensity->setCurrentIndex(1);
    } else if(item->Density() == 90){
        ui->cbxDensity->setCurrentIndex(2);
    } else if(item->Density() == 80){
        ui->cbxDensity->setCurrentIndex(3);
    } else if(item->Density() == 70){
        ui->cbxDensity->setCurrentIndex(4);
    } else if(item->Density() == 60){
        ui->cbxDensity->setCurrentIndex(5);
    } else if(item->Density() == 50){
        ui->cbxDensity->setCurrentIndex(6);
    }

    if(item->Type() == "---"){
        ui->cbxType->setCurrentIndex(0);
    } else if(item->Type() == "Premium"){
        ui->cbxType->setCurrentIndex(1);
    } else if(item->Type() == "Yes"){
        ui->cbxType->setCurrentIndex(2);
    } else if(item->Type() == "Exclusive"){
        ui->cbxType->setCurrentIndex(3);
    } else if(item->Type() == "Sugar" || item->Type() == "Shugar"){
        ui->cbxType->setCurrentIndex(4);
    }

    ui->edtJuice->setText(QString::number(item->Juice()));
    ui->timeFrom->setTime(item->Start());
    ui->timeTo->setTime(item->End());
    ui->edtDimensionFrom->setText(QString::number(item->DimensionFrom()));
    ui->edtDimensionTo->setText(QString::number(item->DimensionTo()));

    ui->edtTemp->setText(QString::number(item->Temperature()));
    ui->textEdit->setText(item->Description());
}

void Dialog::save()
{
    item->setCode_all(ui->edtCode_all->text().toInt());
    item->setCode_year(ui->edtCode_year->text().toInt());
    item->setDate(ui->dateEdit->dateTime());

    if(ui->cbxPerson->currentIndex() != 0){
        item->setPerson(lst.at(ui->cbxPerson->currentIndex()));
    } else {
        item->setPerson(ui->cbxPerson->currentText());
    }

    if(ui->cbxDensity->currentIndex() == 0){
        item->setDensity(0);
    } else if(ui->cbxDensity->currentIndex() == 1){
        item->setDensity(100);
    } else if(ui->cbxDensity->currentIndex() == 2){
        item->setDensity(90);
    } else if(ui->cbxDensity->currentIndex() == 3){
        item->setDensity(80);
    } else if(ui->cbxDensity->currentIndex() == 4){
        item->setDensity(70);
    } else if(ui->cbxDensity->currentIndex() == 5){
        item->setDensity(60);
    } else if(ui->cbxDensity->currentIndex() == 6){
        item->setDensity(70);
    }

    if(ui->cbxType->currentIndex() == 0){
        item->setType("---");
    } else if(ui->cbxType->currentIndex() == 1){
        item->setType("Premium");
    }  else if(ui->cbxType->currentIndex() == 2){
        item->setType("Yes");
    }  else if(ui->cbxType->currentIndex() == 3){
        item->setType("Exclusive");
    }  else if(ui->cbxType->currentIndex() == 4){
        item->setType("Sugar");
    }

    item->setJuice(ui->edtJuice->text().toFloat());
    item->setStart(ui->timeFrom->time());
    item->setEnd(ui->timeTo->time());
    item->setDimensionFrom(ui->edtDimensionFrom->text().toInt());
    item->setDimensionTo(ui->edtDimensionTo->text().toInt());
    item->setTemperature(ui->edtTemp->text().toFloat());
    item->setDescription(ui->textEdit->toPlainText());
}


void Dialog::on_btnCancel_clicked()
{
    close();
}

void Dialog::on_btnAdd_clicked()
{
    save();
    close();
}

void Dialog::fillBoxes()
{
    ui->cbxPerson->addItems(lst);

    ui->cbxDensity->addItem("-------");
    ui->cbxDensity->addItem("0 (bandage)");
    ui->cbxDensity->addItem("1 (ultrasoft)");
    ui->cbxDensity->addItem("2 (soft)");
    ui->cbxDensity->addItem("3 (medium)");
    ui->cbxDensity->addItem("4 (hard)");
    ui->cbxDensity->addItem("5 (extrahard)");

    ui->cbxType->addItem("---");
    ui->cbxType->addItem("Premium");
    ui->cbxType->addItem("Yes");
    ui->cbxType->addItem("Exclusive");
    ui->cbxType->addItem("Sugar");
}

int Dialog::personId()
{
    for(int i = 0; i < lst.count(); i++){
        if(item->Person() == lst.at(i)){
            return i;
        }
    }

    return 0;
}

