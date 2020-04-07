#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <locale.h>

// Global variables
MainWindow* mainWindow = 0;
DrawArea* drawArea = 0;

MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    setlocale(LC_ALL, "C");
    mainWindow = this;
    ui->setupUi(this);

    showCoordRange(
        drawArea->getXMin(), drawArea->getXMax(),
        drawArea->getYMin(), drawArea->getYMax()
    );

    ui->radioPolynom->setChecked(true);
    ui->radioC1Spline->setChecked(false);
    ui->radioC2Spline->setChecked(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_xmin_editingFinished()
{

}

void MainWindow::on_xmax_editingFinished()
{

}

void MainWindow::on_ymin_editingFinished()
{

}

void MainWindow::on_clearButton_clicked()
{
    drawArea->clear();
}

void MainWindow::on_drawButton_clicked()
{
    setCoordinates();
    // Here you can add any code that
    // changes the drawing model (for instance, after
    // adding a number of points by mouse clicks).
    drawArea->onDraw();
}

void MainWindow::on_closeButton_clicked()
{
    QApplication::quit();
}

void MainWindow::setCoordinates() {
    double xmin = ui->xmin->text().toDouble();
    double xmax = ui->xmax->text().toDouble();
    double ymin = ui->ymin->text().toDouble();
    double ymax = ui->ymax->text().toDouble();
    if (xmax <= xmin || ymax <= ymin)
        return;

    // drawArea->setCoordinates(xmin, xmax, ymin);
    // We try to save the relative position of center
    double yCenter = (0. - ymin)/(ymax - ymin);
    double aspect = (double) drawArea->height() / (double) drawArea->width();
    double dx = xmax - xmin;
    double dy = dx*aspect;
    ymin = 0. - dy*yCenter;
    ymax = ymin + dy;

    QString txt;
    txt.sprintf("%.1f", ymin);
    ui->ymin->setText(txt);

    txt.sprintf("%.1f", ymax);
    ui->ymax->setText(txt);

    drawArea->setCoordinates(xmin, xmax, ymin, ymax);
    // drawArea->update();
}

void MainWindow::showCoordRange(
    double xmin, double xmax,
    double ymin, double ymax
) {
    QString txt;
    txt.sprintf("%.1f", xmin);
    ui->xmin->setText(txt);

    txt.sprintf("%.1f", xmax);
    ui->xmax->setText(txt);

    txt.sprintf("%.1f", ymin);
    ui->ymin->setText(txt);

    txt.sprintf("%.1f", ymax);
    ui->ymax->setText(txt);
}


void MainWindow::on_radioPolynom_clicked()
{
    drawArea->whatToDraw = DRAW_POLYNOMIAL;
    drawArea->interpolate();
    drawArea->update();
}

void MainWindow::on_radioC1Spline_clicked()
{
    drawArea->whatToDraw = DRAW_C1_SPLINE;
    if (!drawArea->c1SplineComputed)
        drawArea->computeC1Spline();
    drawArea->c2SplineComputed = false;

    drawArea->update();
}

void MainWindow::on_radioC2Spline_clicked()
{
    drawArea->whatToDraw = DRAW_C2_SPLINE;
    if (!drawArea->c2SplineComputed)
        drawArea->computeC2Spline();
    drawArea->c1SplineComputed = false;

    drawArea->update();
}
