#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class Mainwindow;
}

class Mainwindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit Mainwindow(QWidget *parent = nullptr);
    ~Mainwindow();

private:
    Ui::Mainwindow *ui;
};

#endif // MAINWINDOW_H
