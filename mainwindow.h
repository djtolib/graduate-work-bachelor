#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "RealPixel.h"
#include <time.h>
class DrawArea;
class MainWindow;

extern MainWindow* mainWindow;
extern DrawArea* drawArea;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QString imagePath;
    QImage* image;
    int imageWidth;
    int imageHeight;
    RealPixel* imageMatrix;
    double sigma;
    double radius;
    double zoom;
    RealPixel p[4][4];
    // RealPixel a[4][4],p[4][4]; // a is bicubic interpolation coefficients,
                                  // p is interpol points;
    int modifiedImageWidth;
    int modifiedImageHeight;
    RealPixel* modifiedMatrix;
    QImage* modifiedImage;
    int testImageIdx;
    int splineType;     // 0 == C2-Spline, 1 == C1-Spline

    bool loadImage(QString path);
    void defineImageMatrix();
    void computeModifiedImage(
        int w, int h, const RealPixel* matrix
    );

    void createTestImage(int idx);
    void bilinear_interpolation();

    // Bicubic interpolation
    void   updateCoefficients();
    RealPixel getValue(double, double);
    void bicubic_interpolation();
    RealPixel cubicInterpolate(RealPixel p[], double);
    RealPixel bicubicInterpolate( //double p[4][4],
        double x, double y
    );

    // Spline interpolation
    void onSplineInterpolation();

    void onPixelMixing();
    void onGaussResize();

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_path_returnPressed();

    void on_browseButton_clicked();

    void on_loadButton_clicked();

    void on_black_whiteButton_clicked();

    void on_gaussButton_clicked();

    void on_radio_pixel_mixing_clicked();

    void on_radio_gauss_filter_clicked();

    void on_resize_Button_clicked();

    void on_testImageButton_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_HighPassBtn_clicked();

    void on_highPassBtn_clicked();

    void on_splineButton_clicked();

    void on_radioC1Spline_clicked();

    void on_radioC2Spline_clicked();

    void on_saveToClip_clicked();

private:
    Ui::MainWindow *ui;
};

void createGaussPattern(
    double sigma, int maxSize,
    int& filterSize,
    double** filter,
    double& filterNorm
);

#endif // MAINWINDOW_H
