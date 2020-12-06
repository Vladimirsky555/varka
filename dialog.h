#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

#include "data.h"

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

    Data *item;
    bool edit; //true - добавление, false - редактирование
    QStringList lst;

public:
    Dialog(Data *item, QStringList lst,bool edit, QWidget *parent = 0);
    ~Dialog();

    void load();
    void save();

    void fillBoxes();
    int personId();//Возвращает номер варщика в списке

private slots:
    void on_btnCancel_clicked();
    void on_btnAdd_clicked();

private:
    Ui::Dialog *ui;
};

#endif // DIALOG_H
