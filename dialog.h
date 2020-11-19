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

public:
    Dialog(Data *item, bool edit, QWidget *parent = 0);
    ~Dialog();

    void load();
    void save();

    void fillBoxes();

private slots:
    void on_btnCancel_clicked();
    void on_btnAdd_clicked();

private:
    Ui::Dialog *ui;
};

#endif // DIALOG_H
