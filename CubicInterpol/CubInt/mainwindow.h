#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class DrawArea;

extern DrawArea* drawArea;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void showCoordRange(
        double xmin, double xmax,
        double ymin, double ymax
    );

private:
    void setCoordinates();

private slots:
    void on_xmin_editingFinished();
    void on_xmax_editingFinished();
    void on_ymin_editingFinished();
    void on_clearButton_clicked();
    void on_drawButton_clicked();
    void on_closeButton_clicked();

    void on_radioPolynom_clicked();

    void on_radioC1Spline_clicked();

    void on_radioC2Spline_clicked();

private:
    Ui::MainWindow *ui;
};

extern MainWindow* mainWindow;

#endif // MAINWINDOW_H
