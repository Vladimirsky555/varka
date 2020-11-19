#ifndef SHOWITEMWINDOW_H
#define SHOWITEMWINDOW_H

#include <QWidget>

#include "data.h"
#include "showitemwindow.h"
#include "regexphighlighter.h"

namespace Ui {
class ShowItemWindow;
}

class ShowItemWindow : public QWidget
{
    Q_OBJECT

    Data* item;   
    RegexpHighlighter *highlighter;

public:
    explicit ShowItemWindow(Data* item, QString pattern, QString description, QWidget *parent = 0);
    ~ShowItemWindow();

    void setPerson(QString str);

private slots:
    void shutdown();

private:
    Ui::ShowItemWindow *ui;
};

#endif // SHOWITEMWINDOW_H
