#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QJsonObject>

#include "data.h"
#include "dialog.h"
#include "model.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    Model *M;
    QJsonObject m_currentJsonObject;
    QStringList lst;//Список варщиков

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void createUI();
    void fillBoxes();
    bool check_index(int index);
    void printList();


private slots:
    void on_addVarka_clicked();
    void on_tableView_clicked(const QModelIndex &index);
    void on_btnAll_clicked();
    void on_btnSearch_clicked();
    void on_edtPattern_m_textChanged(const QString &str);
    void clearBoxes();
    void ExportToJSON();


signals:
    void sendIndextoModel(QModelIndex index);
    void sendPattern(QString);//для подстветки
    void shutdown();

private:
    Ui::MainWindow *ui;

    // QWidget interface
protected:
    virtual void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
