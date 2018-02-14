#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ai.h"
#include "map.h"
#include "mapfunctions.h"

#include <unordered_map>

#include <QMainWindow>

namespace Ui {

class SquareWidget : public QWidget {
public:
    SquareWidget(Map *m, std::unordered_map<std::array<int, 2>, int, hashing_func, key_equal_func> *hitcount, int row, int col);
    void redraw_if(int current_location);
protected:
    void paintEvent(QPaintEvent*);
    Map *m;
    std::unordered_map<std::array<int, 2>, int, hashing_func, key_equal_func> *hitcount;
    int row, col;
};

class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    std::unordered_map<std::array<int, 2>, int, hashing_func, key_equal_func> hitcount;
    Map m;
    AI *ai;
    int previous_state;
};

#endif // MAINWINDOW_H
