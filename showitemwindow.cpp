#include "showitemwindow.h"
#include "ui_showitemwindow.h"

#include <QMessageBox>

ShowItemWindow::ShowItemWindow(Data *item, QString pattern, QString description, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShowItemWindow)
{
    ui->setupUi(this);

    this->item = item;

    //Подсветка поисковой фразы
    highlighter = new RegexpHighlighter(this);
    highlighter->setDocument(ui->edtDescription->document());
    highlighter->setPattern(pattern);
    highlighter->rehighlight();

    setWindowTitle(item->Date().toString("dd.MM.yyyy") + " - " +
                   item->Person() + " - " + item->Type() +
                   " [" + this->item->Density_plus() + "]");

    if(item->Person() == "Артём"){
        setPerson(":/images/artem.jpg");
    } else if(item->Person() == "Владимир"){
        setPerson(":/images/vladimir.jpg");
    } else if(item->Person() == "Александр"){
        setPerson(":/images/alexander.jpg");
    } else {
        setPerson(":/images/anonim.jpg");
    }

    ui->dateEdit->setReadOnly(true);
    ui->edtNum->setReadOnly(true);
    ui->edtType->setReadOnly(true);
    ui->edtDensity->setReadOnly(true);
    ui->edtJuice->setReadOnly(true);
    ui->edtMeasurement->setReadOnly(true);
    ui->timeFrom->setReadOnly(true);
    ui->timeTo->setReadOnly(true);

    ui->dateEdit->setDateTime(item->Date());
    ui->lblPerson->setText(item->Person());
    ui->edtNum->setText(QString::number(item->Code_all()) + " / " + QString::number(item->Code_year()));
    ui->edtType->setText(item->Type());
      ui->edtDensity->setText(item->Density_plus());
    ui->edtJuice->setText(QString::number(item->Juice()));
    ui->edtDescription->setHtml(description);

    if(item->DimensionFrom() == 0){
        ui->edtMeasurement->setText("Нет");
    } else {
        ui->edtMeasurement->setText(QString::number(item->DimensionFrom()) + "-" +
                                    QString::number(item->DimensionTo()) + " t = " +
                                    QString::number(item->Temperature()) + "°");
    }

    ui->timeFrom->setTime(item->Start());
    ui->timeTo->setTime(item->End());

}

ShowItemWindow::~ShowItemWindow()
{
    delete ui;
}


//Ставим фотку на просмотр варки
void ShowItemWindow::setPerson(QString str)
{
    QPixmap pix(str);
    ui->lblPix->setPixmap(pix.scaledToWidth(200));
}

//Закрываем это окно, если основное закрывается
void ShowItemWindow::shutdown()
{
    close();
}


