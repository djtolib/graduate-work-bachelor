#include "mainwindow.h"
#include <cmath>
#include "ui_mainwindow.h"
#include "drawarea.h"
#include <QFileDialog>
#include <QPainter>
#include <QPainterPath>
#include <qmath.h>
#include <QDebug>
#include <cassert>
#include <QMessageBox>
#include <QClipboard>

const int NUM_TEST_IMAGES = 3;
const int TEST_IMAGE_WIDTH = 800;
const int TEST_IMAGE_HEIGHT = 600;

const double PI = 3.14159265358979323846;

MainWindow* mainWindow = 0;
DrawArea* drawArea = 0;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    imagePath(),
    image(0),
    imageWidth(0),
    imageHeight(0),
    imageMatrix(0),
    sigma(1.),
    radius(5.),
    zoom(4.),
    modifiedImageWidth(0),
    modifiedImageHeight(0),
    modifiedMatrix(0),
    modifiedImage(0),
    testImageIdx(0),
    splineType(1),      // C1-Spline
    ui(new Ui::MainWindow)
{
    mainWindow = this;
    ui->setupUi(this);
    QString txt;

    txt.sprintf("%f", sigma);
    ui->sigma_edit->setText(txt);
    txt.sprintf("%d", (int) radius);
    ui->radius_edit->setText(txt);
    if(ui->radio_pixel_mixing->isChecked()){
        ui->gaussButton->setEnabled(false);
        ui->sigma_edit->setEnabled(false);
    }
    ui->sigmaLabel->hide();
    ui->sigma_edit->hide();
    ui->gaussButton->hide();
    ui->radiusLabel->hide();
    ui->radius_edit->hide();
    ui->radio_gauss_filter->hide();
    ui->radio_pixel_mixing->hide();
    txt.sprintf("%f", zoom);
    ui->coeff_resize->setText(txt);
    ui->black_whiteButton->setVisible(true);
    ui->radioC1Spline->setChecked(true);
    ui->radioC2Spline->setChecked(false);
}

MainWindow::~MainWindow()
{
    delete image;
    delete[] imageMatrix;
    delete[] modifiedMatrix;
    delete ui;
}

void MainWindow::on_path_returnPressed()
{
    imagePath = ui->path->text();
    on_loadButton_clicked();
}

void MainWindow::on_browseButton_clicked()
{
    QFileDialog openFileDlg;
    openFileDlg.setDirectory( QDir("C:/Users/Tolibjon/Desktop/diploma/Borisen/ImView-last/Test/"));
    QString fileName = QFileDialog::getOpenFileName(
        this, QString("Open Image")
    );
    if (fileName == QString())
        return;
    ui->path->setText(fileName);
    on_loadButton_clicked();
}

void MainWindow::on_loadButton_clicked()
{
    // image = new QImage(w, h, QImage::Format_RGB32);
    imagePath = ui->path->text();
    //... image = new QImage(imagePath);
    if (image != 0)
        delete image;
    image = new QImage();
    if (!image->load(imagePath)) {
        delete image; image = 0;
        return;
    }

    if (modifiedImage != 0) {
        delete modifiedImage;
        modifiedImage = 0;
    }

    imageWidth = image->width();
    imageHeight = image->height();

    defineImageMatrix();
    drawArea->update();
}

void MainWindow::defineImageMatrix() {
    if (image == 0)
        return;

    delete[] imageMatrix;   // удаление ранее созданный матрицы
    imageMatrix = new RealPixel[(imageWidth+2) * (imageHeight+2)]; // Создание новой матрицы
    for (int y = 0; y < imageHeight; ++y) {
        for (int x = 0; x < imageWidth; ++x) {
            QRgb pixval = image->pixel(x, y);                // перевод из типа QRgb
            imageMatrix[y*imageWidth + x] = RealPixel(       // в RealPixel
                qRed(pixval), qGreen(pixval), qBlue(pixval)  // и заполнение новой матрицы
            );
        }
    }
}

void MainWindow::on_black_whiteButton_clicked()
{

    if (imageMatrix == 0)
        return;
    RealPixel *currMatrix;
    int currWidth, currHeight;
    if (modifiedMatrix != 0)
        {
            currMatrix = modifiedMatrix;
            currWidth  = modifiedImageWidth;
            currHeight = modifiedImageHeight;
        }
    else
        {
            currMatrix = imageMatrix;
            currWidth  = imageWidth;
            currHeight = imageHeight;
        }
    RealPixel* matrix = new RealPixel[currWidth*currHeight];
    for (int y = 0; y < currHeight; ++y) {
        const RealPixel* srccurrRow = currMatrix + y*currWidth;
        RealPixel* dstcurrRow = matrix + y*currWidth;
        for (int x = 0; x < currWidth; ++x) {
            const RealPixel* srcPixel = srccurrRow + x;
            RealPixel* dstPixel = dstcurrRow + x;
            double s = 0.2126*srcPixel->red() + 0.7152*srcPixel->green() + 0.0722*srcPixel->blue();
//            s /= 3.;
            *dstPixel = RealPixel(s, s, s);
        }
    }

    if (modifiedImage != 0)
        delete modifiedImage;

    modifiedImage = new QImage(
        currWidth, currHeight, QImage::Format_RGB32
    );
    modifiedImageWidth = currWidth;
    modifiedImageHeight = currHeight;
    for (int y = 0; y < currHeight; ++y) {
        const RealPixel* srcImageRow = matrix + y*currWidth;
        QRgb* dstImageRow = (QRgb*)(modifiedImage->scanLine(y));
        for (int x = 0; x < currWidth; ++x) {
            const RealPixel* srcPixel = srcImageRow + x;
            dstImageRow[x] = qRgb(
                srcPixel->red255(),
                srcPixel->green255(),
                srcPixel->blue255()
            );
        }
    }


    if (image != 0) {
        *image = *modifiedImage;
        defineImageMatrix();
        memmove(
             imageMatrix, matrix,
             imageWidth*imageHeight*sizeof(RealPixel)
        );
    }
    delete[] matrix;

    drawArea->update();
}

void MainWindow::computeModifiedImage(
    int w, int h, const RealPixel* matrix
) {
    if (modifiedImage != 0) {
        delete modifiedImage;
        modifiedImage = 0;
    }

    modifiedImageWidth = w;
    modifiedImageHeight = h;
    modifiedImage = new QImage(
        w, h, QImage::Format_RGB32
    );
    for (int y = 0; y < h; ++y) {
        const RealPixel* srcImageRow = matrix + y*w;
        QRgb* dstImageRow = (QRgb*)(modifiedImage->scanLine(y));
        for (int x = 0; x < w; ++x) {
            const RealPixel* srcPixel = srcImageRow + x;
            dstImageRow[x] = qRgb(
                srcPixel->red255(),
                srcPixel->green255(),
                srcPixel->blue255()
            );
        }
    }
}

void MainWindow::on_gaussButton_clicked()
{
    if (ui->sigma_edit->text() != "")
        sigma = ui->sigma_edit->text().toDouble();
    if (ui->radius_edit->text() != "")
        radius = ui->radius_edit->text().toDouble();
    if (sigma <= 0.)
        return;
    if (imageMatrix == 0)
        return;

    setCursor(QCursor(Qt::WaitCursor));

    double *filter;
    int filterSize;
    double filterNorm;

    createGaussPattern(
        sigma, radius, filterSize, &filter, filterNorm
    );

    int s = filterSize/2;

    delete[] modifiedMatrix;
    modifiedMatrix = new RealPixel[imageWidth * imageHeight];
    for (int y = 0; y < imageHeight; ++y) {
        RealPixel* dstImageRow = modifiedMatrix + y*imageWidth;
        for(int x = 0; x < imageWidth; x++){
            RealPixel& dstPixel = dstImageRow[x];
            dstPixel = RealPixel(0., 0., 0.);

            double red = 0., green = 0., blue = 0.;
            double norm = 0.;
            for (int dy = (-s); dy <= s; ++dy) {
                int y1 = y + dy;
                if (y1 < 0 || y1 >= imageHeight)
                    continue;
                for (int dx = (-s); dx <= s; ++dx) {
                    int x1 = x + dx;
                    if (x1 < 0 || x1 >= imageWidth)
                        continue;
                    const RealPixel& srcPixel =
                        imageMatrix[y1*imageWidth + x1];
                    double v = filter[(dy+s)*filterSize + (dx+s)];
                    norm += v;
                    red += srcPixel.red()*v;
                    green += srcPixel.green()*v;
                    blue += srcPixel.blue()*v;
                }
            }
            if (norm <= 0.)
                continue;
            red /= norm;
            green /= norm;
            blue /= norm;
            dstPixel = RealPixel(red, green, blue);
        }
    }
    delete[] filter;
    computeModifiedImage(
        imageWidth, imageHeight, modifiedMatrix
    );

    setCursor(QCursor(Qt::ArrowCursor));
    drawArea->update();
}

void createGaussPattern(
    double sigma, int maxSize,
    int& filterSize,
    double** filter,
    double& filterNorm
) {
    assert(sigma > 0.);
    double sigma22 =2.*sigma*sigma;
    double pi2sigma = sigma*sqrt(2.*M_PI);
    int s = (int)(1. + 2.*sigma);
    if (s < 3)
        s = 3;
    s |= 1; // Make odd
    if (s > maxSize) {
        s = maxSize;
        s |= 1;
    }
    int center = (s-1)/2;
    filterSize = s;
    double* flt = new double[s*s];
    *filter = flt;
    double norm = 0.;
    for (int y = 0; y < s; ++y) {
        int dy = y - center;
        for (int x = 0; x < s; ++x) {
            int dx = x - center;
            double r = (double)((dx - dy*dy)*(dx - dy*dy));
            double v = exp(-r/(sigma22))/pi2sigma;
            norm += v;
            flt[y*s + x] = v;
        }
    }
    filterNorm = norm;
}



void MainWindow::on_radio_pixel_mixing_clicked()
{
    ui->gaussButton->setEnabled(false);
    ui->sigma_edit->setEnabled(false);

}

void MainWindow::on_radio_gauss_filter_clicked()
{

    ui->sigma_edit->setEnabled(true);
   // ui->gaussButton->setEnabled(true);
}

void MainWindow::on_resize_Button_clicked()
{
    zoom = ui->coeff_resize->text().toDouble();
    zoom = fabs(zoom);
//    if (zoom > 1. || fabs(zoom) <= 0.) {
//        zoom = 1.;
//        ui->coeff_resize->setText("1.0");

//        on_gaussButton_clicked();
//        return;
//    }
    QString txt;
    txt.sprintf("%.6f", zoom);
    ui->coeff_resize->setText(txt);

    if (ui->radio_pixel_mixing->isChecked()) {
        onPixelMixing();
    } else {
        onGaussResize();
    }
}

void MainWindow::onPixelMixing() {
    if (imageMatrix == 0)
        return;

    setCursor(QCursor(Qt::WaitCursor));

    modifiedImageWidth = (int)((double) imageWidth * zoom);
    modifiedImageHeight = (int)((double) imageHeight * zoom);

    delete[] modifiedMatrix;
    modifiedMatrix = new RealPixel[
        modifiedImageWidth * modifiedImageHeight
    ];

    double square = 0.5/zoom;

    int y, yDst;
    double Y0 = 0.;
    double Y1 = square;
    if (Y1 > (double) imageHeight)
        Y1 = (double) imageHeight;
    double DY = Y1 - Y0;
    for (yDst = 0; yDst < modifiedImageHeight; yDst++)
    {
        double X0 = 0.;
        double X1 = square;
        if (X1 > (double) imageWidth)
            X1 = (double) imageWidth;
        double DS = (X1 - X0) * DY;
        int x, xDst;
        for (xDst = 0; xDst < modifiedImageWidth; xDst++)
        {
            double vRed = 0.;
            double vGreen = 0.;
            double vBlue = 0.;

            double y_low = Y0;
            double y_high;
            if (y_low == (double)((int) y_low))
                y_high = y_low + 1.;
            else
                y_high = ceil(y_low);
            double dy = y_high - y_low;
            y = (int) y_low;

            while (y_low < Y1)
            {
                double x_low = X0;
                double x_high;
                double ds;
                if (x_low == (double)((int) x_low))
                {
                    x_high = x_low + 1.;
                    ds = dy;
                }
                else
                {
                    x_high = ceil(x_low);
                    ds = (x_high - x_low) * dy;
                }
                x = (int) x_low;

                while (x_low < X1)
                {
                    vRed += imageMatrix[y*imageWidth + x].red() * ds;
                    vGreen += imageMatrix[y*imageWidth + x].green() * ds;
                    vBlue += imageMatrix[y*imageWidth + x].blue() * ds;
                    x_low = x_high;
                    x_high += 1.;
                    if (x_high > X1)
                    {
                        x_high = X1;
                        ds = (x_high - x_low) * dy;
                    }
                    else
                    {
                        ds = dy;
                    }
                    x++;
                } // end while (x_low < X1)

                y_low = y_high;
                y_high += 1.;
                if (y_high > Y1)
                {
                    y_high = Y1;
                    dy = y_high - y_low;
                }
                else
                {
                    dy = 1.;
                }
                y++;
            } // end while (y_low < Y1)

            modifiedMatrix[yDst*modifiedImageWidth + xDst].setRed(
                vRed / DS
            );
            modifiedMatrix[yDst*modifiedImageWidth + xDst].setGreen(
                vGreen / DS
            );
            modifiedMatrix[yDst*modifiedImageWidth + xDst].setBlue(
                vBlue / DS
            );

            X0 = X1;
            X1 += square;
            if (X1 > (double) imageWidth)
            {
                X1 = (double) imageWidth;
                DS = (X1 - X0) * DY;
            }
            else
            {
                DS = square * DY;
            }
        } // end for (xDst...
        Y0 = Y1;
        Y1 += square;
        if (Y1 > (double) imageHeight)
        {
            Y1 = (double) imageHeight;
            DY = Y1 - Y0;
        }
        else
        {
            DY = square;
        }
    } // end for (yDst...

    computeModifiedImage(
        modifiedImageWidth, modifiedImageHeight, modifiedMatrix
    );

    setCursor(QCursor(Qt::ArrowCursor));
    drawArea->update();
}

void MainWindow::onGaussResize() {
    //... if (!ui->radio_gauss_filter->isChecked())
    //...     return;
    if (ui->sigma_edit->text() != "")
        sigma = ui->sigma_edit->text().toDouble();
    if (ui->radius_edit->text() != "")
        radius = ui->radius_edit->text().toDouble();
    if (sigma <= 0.)
        return;
    if (imageMatrix == 0)
        return;

    setCursor(QCursor(Qt::WaitCursor));

    double *filter;
    int filterSize;
    double filterNorm;

    createGaussPattern(
        sigma, radius, filterSize, &filter, filterNorm
    );

    int s = filterSize/2;

    RealPixel* tmpMatrix = new RealPixel[imageWidth * imageHeight];
    for (int y = 0; y < imageHeight; ++y) {
        RealPixel* dstImageRow = tmpMatrix + y*imageWidth;
        for(int x = 0; x < imageWidth; x++){
            RealPixel& dstPixel = dstImageRow[x];
            dstPixel = RealPixel(0., 0., 0.);

            double red = 0., green = 0., blue = 0.;
            double norm = 0.;
            for (int dy = (-s); dy <= s; ++dy) {
                int y1 = y + dy;
                if (y1 < 0 || y1 >= imageHeight)
                    continue;
                for (int dx = (-s); dx <= s; ++dx) {
                    int x1 = x + dx;
                    if (x1 < 0 || x1 >= imageWidth)
                        continue;
                    const RealPixel& srcPixel =
                        imageMatrix[y1*imageWidth + x1];
                    double v = filter[(dy+s)*filterSize + (dx+s)];
                    norm += v;
                    red += srcPixel.red()*v;
                    green += srcPixel.green()*v;
                    blue += srcPixel.blue()*v;
                }
            }
            if (norm <= 0.)
                continue;
            red /= norm;
            green /= norm;
            blue /= norm;
            dstPixel = RealPixel(red, green, blue);
        }
    }
    delete[] filter;

    modifiedImageWidth = (int)((double) imageWidth * zoom);
    modifiedImageHeight = (int)((double) imageHeight * zoom);

    delete[] modifiedMatrix;
    modifiedMatrix = new RealPixel[
        modifiedImageWidth * modifiedImageHeight
    ];

    double invZoom = 1./zoom;
    for (int y = 0; y < modifiedImageHeight; ++y) {
        double ySrc = invZoom * (double) y;
        int y0 = (int) ySrc;
        double wy0 = 1. - (ySrc - (double) y0);
        int y1 = y0 + 1;
        double wy1 = 1. - wy0;
        if (y1 >= imageHeight) {
            y1 = y0;
            wy0 = 1.; wy1 = 0.;
        }
        for (int x = 0; x < modifiedImageWidth; ++x) {
            double xSrc = invZoom * (double) x;
            int x0 = (int) xSrc;
            double wx0 = 1. - (xSrc - (double) x0);
            int x1 = x0 + 1;
            double wx1 = 1. - wx0;
            if (x1 >= imageWidth) {
                x1 = y0;
                wx0 = 1.; wx1 = 0.;
            }
            double vy0x0 = tmpMatrix[y0*imageWidth + x0].red();
            double vy0x1 = tmpMatrix[y0*imageWidth + x1].red();
            double vy1x0 = tmpMatrix[y1*imageWidth + x0].red();
            double vy1x1 = tmpMatrix[y1*imageWidth + x1].red();
            double vx0 = vy0x0*wy0 + vy1x0*wy1;
            double vx1 = vy0x1*wy0 + vy1x1*wy1;
            double v = vx0*wx0 + vx1*wx1;
            modifiedMatrix[y*modifiedImageWidth + x].setRed(v);

            vy0x0 = tmpMatrix[y0*imageWidth + x0].green();
            vy0x1 = tmpMatrix[y0*imageWidth + x1].green();
            vy1x0 = tmpMatrix[y1*imageWidth + x0].green();
            vy1x1 = tmpMatrix[y1*imageWidth + x1].green();
            vx0 = vy0x0*wy0 + vy1x0*wy1;
            vx1 = vy0x1*wy0 + vy1x1*wy1;
            v = vx0*wx0 + vx1*wx1;
            modifiedMatrix[y*modifiedImageWidth + x].setGreen(v);

            vy0x0 = tmpMatrix[y0*imageWidth + x0].blue();
            vy0x1 = tmpMatrix[y0*imageWidth + x1].blue();
            vy1x0 = tmpMatrix[y1*imageWidth + x0].blue();
            vy1x1 = tmpMatrix[y1*imageWidth + x1].blue();
            vx0 = vy0x0*wy0 + vy1x0*wy1;
            vx1 = vy0x1*wy0 + vy1x1*wy1;
            v = vx0*wx0 + vx1*wx1;
            modifiedMatrix[y*modifiedImageWidth + x].setBlue(v);
        }
    }

    delete[] tmpMatrix;

    computeModifiedImage(
        modifiedImageWidth, modifiedImageHeight, modifiedMatrix
    );

    setCursor(QCursor(Qt::ArrowCursor));
    drawArea->update();
}

void MainWindow::on_testImageButton_clicked()
{
    createTestImage(testImageIdx);
    ++testImageIdx;
    if (testImageIdx >= NUM_TEST_IMAGES)
        testImageIdx = 0;
}

void MainWindow::createTestImage(int idx) {
    if (image != 0) {
        delete image;
    }
    imageWidth = TEST_IMAGE_WIDTH;
    imageHeight = TEST_IMAGE_HEIGHT;

    image = new QImage(
        imageWidth, imageHeight, QImage::Format_RGB32
    );
    QPainter painter(image);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(0, 0, imageWidth, imageHeight, Qt::white);
    QPen pen(Qt::black, 1);
    if (idx == 0) {
        QPoint center(imageWidth/2, imageHeight/2);
        int dr = 4;
        painter.setPen(pen);
        painter.setBrush(Qt::NoBrush);
        int r = dr;
        while (r < imageWidth/2) {
            painter.drawEllipse(center, r, r);
            r += dr;
        }
    } else if (idx == 1) {
        QPointF centerF(imageWidth/2, imageHeight/2);
        painter.setPen(pen);
        painter.setBrush(Qt::NoBrush);
        double alpha = 0.;
        double dAlpha = 3.* (PI*2./360.);   // 3 degrees
        double rx = (double) imageWidth/2. + 0.001;
        double ry = (double) imageHeight/2. + 0.001;
        double r = sqrt(rx*rx + ry*ry);
        while (alpha < (2.*PI - 1e-8)) {
            QPointF p(r*cos(alpha), r*sin(alpha));
            p += centerF;
            painter.drawLine(centerF, p);
            alpha += dAlpha;
        }
    } else if (idx == 2) {
        double xmax = (double) TEST_IMAGE_WIDTH;
        double ymax = (double) TEST_IMAGE_HEIGHT;
        double ax = 10.;
        double ay = ax;
        double dx = 1.;
        double dy = 5.;
        double y = ay + 2.;
        while (y < ymax - ay - 2.) {
            double x = 0.;
            QPainterPath path;
            path.moveTo(x, ay*sin(x/ax) + y);
            while (x < xmax) {
                x += dx;
                path.lineTo(x, ay*sin(x/ax) + y);
            }
            painter.strokePath(path, pen);
            y += dy;
        }
    }

    defineImageMatrix();

    delete modifiedImage; modifiedImage = 0;

    drawArea->update();
}

/** BICUBIC INTERPOLATION **/
void MainWindow::updateCoefficients()
{
//    a[0][0].setRed( p[1][1].red());
//    a[0][1].setRed( -.5 * p[1][0].red()+ .5 * p[1][2].red());
//    a[0][2].setRed( p[1][0].red()- 2.5 * p[1][1].red()+ 2 * p[1][2].red()- .5 * p[1][3].red());
//    a[0][3].setRed( -.5 * p[1][0].red()+ 1.5 * p[1][1].red()- 1.5 * p[1][2].red()+ .5 * p[1][3].red());
//    a[1][0].setRed( -.5 * p[0][1].red()+ .5 * p[2][1].red());
//    a[1][1].setRed( .25 * p[0][0].red()- .25 * p[0][2].red()- .25 * p[2][0].red()+ .25 * p[2][2].red());
//    a[1][2].setRed( -.5 * p[0][0].red()+ 1.25 * p[0][1].red()- p[0][2].red()+ .25 * p[0][3].red()
//                + .5 * p[2][0].red()- 1.25 * p[2][1].red()+ p[2][2].red()- .25 * p[2][3].red());
//    a[1][3].setRed( .25 * p[0][0].red()- .75 * p[0][1].red()+ .75 * p[0][2].red()- .25 * p[0][3].red()
//                - .25 * p[2][0].red()+ .75 * p[2][1].red()- .75 * p[2][2].red()+ .25 * p[2][3].red());
//    a[2][0].setRed( p[0][1].red()- 2.5 * p[1][1].red()+ 2 * p[2][1].red()- .5 * p[3][1].red());
//    a[2][1].setRed( -.5 * p[0][0].red()+ .5 * p[0][2].red()+ 1.25 * p[1][0].red()- 1.25 * p[1][2].red()
//                - p[2][0].red()+ p[2][2].red()+ .25 * p[3][0].red()- .25 * p[3][2].red());
//    a[2][2].setRed( p[0][0].red()- 2.5 * p[0][1].red()+ 2 * p[0][2].red()- .5 * p[0][3].red()- 2.5 * p[1][0].red()
//                + 6.25 * p[1][1].red()- 5 * p[1][2].red()+ 1.25 * p[1][3].red()+ 2 * p[2][0].red()
//                - 5 * p[2][1].red()+ 4 * p[2][2].red()- p[2][3].red()- .5 * p[3][0].red()
//                + 1.25 * p[3][1].red()- p[3][2].red()+ .25 * p[3][3].red());
//    a[2][3].setRed( -.5 * p[0][0].red()+ 1.5 * p[0][1].red()- 1.5 * p[0][2].red()+ .5 * p[0][3].red()
//                + 1.25 * p[1][0].red()- 3.75 * p[1][1].red()+ 3.75 * p[1][2].red()
//                - 1.25 * p[1][3].red()- p[2][0].red()+ 3 * p[2][1].red()- 3 * p[2][2].red()+ p[2][3].red()
//                + .25 * p[3][0].red()- .75 * p[3][1].red()+ .75 * p[3][2].red()- .25 * p[3][3].red());
//    a[3][0].setRed( -.5 * p[0][1].red()+ 1.5 * p[1][1].red()- 1.5 * p[2][1].red()+ .5 * p[3][1].red());
//    a[3][1].setRed( .25 * p[0][0].red()- .25 * p[0][2].red()- .75 * p[1][0].red()+ .75 * p[1][2].red()
//                + .75 * p[2][0].red()- .75 * p[2][2].red()- .25 * p[3][0].red()+ .25 * p[3][2].red());
//    a[3][2].setRed( -.5 * p[0][0].red()+ 1.25 * p[0][1].red()- p[0][2].red()+ .25 * p[0][3].red()
//                + 1.5 * p[1][0].red()- 3.75 * p[1][1].red()+ 3 * p[1][2].red()- .75 * p[1][3].red()
//                - 1.5 * p[2][0].red()+ 3.75 * p[2][1].red()- 3 * p[2][2].red()+ .75 * p[2][3].red()
//                + .5 * p[3][0].red()- 1.25 * p[3][1].red()+ p[3][2].red()- .25 * p[3][3].red());
//    a[3][3].setRed( .25 * p[0][0].red()- .75 * p[0][1].red()+ .75 * p[0][2].red()- .25 * p[0][3].red()
//                - .75 * p[1][0].red()+ 2.25 * p[1][1].red()- 2.25 * p[1][2].red()+ .75 * p[1][3].red()
//                + .75 * p[2][0].red()- 2.25 * p[2][1].red()+ 2.25 * p[2][2].red()- .75 * p[2][3].red()
//                - .25 * p[3][0].red()+ .75 * p[3][1].red()- .75 * p[3][2].red()+ .25 * p[3][3].red());

//    a[0][0].setGreen( p[1][1].green());
//    a[0][1].setGreen( -.5 * p[1][0].green()+ .5 * p[1][2].green());
//    a[0][2].setGreen( p[1][0].green()- 2.5 * p[1][1].green()+ 2 * p[1][2].green()- .5 * p[1][3].green());
//    a[0][3].setGreen( -.5 * p[1][0].green()+ 1.5 * p[1][1].green()- 1.5 * p[1][2].green()+ .5 * p[1][3].green());
//    a[1][0].setGreen( -.5 * p[0][1].green()+ .5 * p[2][1].green());
//    a[1][1].setGreen( .25 * p[0][0].green()- .25 * p[0][2].green()- .25 * p[2][0].green()+ .25 * p[2][2].green());
//    a[1][2].setGreen( -.5 * p[0][0].green()+ 1.25 * p[0][1].green()- p[0][2].green()+ .25 * p[0][3].green()
//                + .5 * p[2][0].green()- 1.25 * p[2][1].green()+ p[2][2].green()- .25 * p[2][3].green());
//    a[1][3].setGreen( .25 * p[0][0].green()- .75 * p[0][1].green()+ .75 * p[0][2].green()- .25 * p[0][3].green()
//                - .25 * p[2][0].green()+ .75 * p[2][1].green()- .75 * p[2][2].green()+ .25 * p[2][3].green());
//    a[2][0].setGreen( p[0][1].green()- 2.5 * p[1][1].green()+ 2 * p[2][1].green()- .5 * p[3][1].green());
//    a[2][1].setGreen( -.5 * p[0][0].green()+ .5 * p[0][2].green()+ 1.25 * p[1][0].green()- 1.25 * p[1][2].green()
//                - p[2][0].green()+ p[2][2].green()+ .25 * p[3][0].green()- .25 * p[3][2].green());
//    a[2][2].setGreen( p[0][0].green()- 2.5 * p[0][1].green()+ 2 * p[0][2].green()- .5 * p[0][3].green()- 2.5 * p[1][0].green()
//                + 6.25 * p[1][1].green()- 5 * p[1][2].green()+ 1.25 * p[1][3].green()+ 2 * p[2][0].green()
//                - 5 * p[2][1].green()+ 4 * p[2][2].green()- p[2][3].green()- .5 * p[3][0].green()
//                + 1.25 * p[3][1].green()- p[3][2].green()+ .25 * p[3][3].green());
//    a[2][3].setGreen( -.5 * p[0][0].green()+ 1.5 * p[0][1].green()- 1.5 * p[0][2].green()+ .5 * p[0][3].green()
//                + 1.25 * p[1][0].green()- 3.75 * p[1][1].green()+ 3.75 * p[1][2].green()
//                - 1.25 * p[1][3].green()- p[2][0].green()+ 3 * p[2][1].green()- 3 * p[2][2].green()+ p[2][3].green()
//                + .25 * p[3][0].green()- .75 * p[3][1].green()+ .75 * p[3][2].green()- .25 * p[3][3].green());
//    a[3][0].setGreen( -.5 * p[0][1].green()+ 1.5 * p[1][1].green()- 1.5 * p[2][1].green()+ .5 * p[3][1].green());
//    a[3][1].setGreen( .25 * p[0][0].green()- .25 * p[0][2].green()- .75 * p[1][0].green()+ .75 * p[1][2].green()
//                + .75 * p[2][0].green()- .75 * p[2][2].green()- .25 * p[3][0].green()+ .25 * p[3][2].green());
//    a[3][2].setGreen( -.5 * p[0][0].green()+ 1.25 * p[0][1].green()- p[0][2].green()+ .25 * p[0][3].green()
//                + 1.5 * p[1][0].green()- 3.75 * p[1][1].green()+ 3 * p[1][2].green()- .75 * p[1][3].green()
//                - 1.5 * p[2][0].green()+ 3.75 * p[2][1].green()- 3 * p[2][2].green()+ .75 * p[2][3].green()
//                + .5 * p[3][0].green()- 1.25 * p[3][1].green()+ p[3][2].green()- .25 * p[3][3].green());
//    a[3][3].setGreen( .25 * p[0][0].green()- .75 * p[0][1].green()+ .75 * p[0][2].green()- .25 * p[0][3].green()
//                - .75 * p[1][0].green()+ 2.25 * p[1][1].green()- 2.25 * p[1][2].green()+ .75 * p[1][3].green()
//                + .75 * p[2][0].green()- 2.25 * p[2][1].green()+ 2.25 * p[2][2].green()- .75 * p[2][3].green()
//                - .25 * p[3][0].green()+ .75 * p[3][1].green()- .75 * p[3][2].green()+ .25 * p[3][3].green());

//    a[0][0].setBlue( p[1][1].blue());
//    a[0][1].setBlue( -.5 * p[1][0].blue()+ .5 * p[1][2].blue());
//    a[0][2].setBlue( p[1][0].blue()- 2.5 * p[1][1].blue()+ 2 * p[1][2].blue()- .5 * p[1][3].blue());
//    a[0][3].setBlue( -.5 * p[1][0].blue()+ 1.5 * p[1][1].blue()- 1.5 * p[1][2].blue()+ .5 * p[1][3].blue());
//    a[1][0].setBlue( -.5 * p[0][1].blue()+ .5 * p[2][1].blue());
//    a[1][1].setBlue( .25 * p[0][0].blue()- .25 * p[0][2].blue()- .25 * p[2][0].blue()+ .25 * p[2][2].blue());
//    a[1][2].setBlue( -.5 * p[0][0].blue()+ 1.25 * p[0][1].blue()- p[0][2].blue()+ .25 * p[0][3].blue()
//                + .5 * p[2][0].blue()- 1.25 * p[2][1].blue()+ p[2][2].blue()- .25 * p[2][3].blue());
//    a[1][3].setBlue( .25 * p[0][0].blue()- .75 * p[0][1].blue()+ .75 * p[0][2].blue()- .25 * p[0][3].blue()
//                - .25 * p[2][0].blue()+ .75 * p[2][1].blue()- .75 * p[2][2].blue()+ .25 * p[2][3].blue());
//    a[2][0].setBlue( p[0][1].blue()- 2.5 * p[1][1].blue()+ 2 * p[2][1].blue()- .5 * p[3][1].blue());
//    a[2][1].setBlue( -.5 * p[0][0].blue()+ .5 * p[0][2].blue()+ 1.25 * p[1][0].blue()- 1.25 * p[1][2].blue()
//                - p[2][0].blue()+ p[2][2].blue()+ .25 * p[3][0].blue()- .25 * p[3][2].blue());
//    a[2][2].setBlue( p[0][0].blue()- 2.5 * p[0][1].blue()+ 2 * p[0][2].blue()- .5 * p[0][3].blue()- 2.5 * p[1][0].blue()
//                + 6.25 * p[1][1].blue()- 5 * p[1][2].blue()+ 1.25 * p[1][3].blue()+ 2 * p[2][0].blue()
//                - 5 * p[2][1].blue()+ 4 * p[2][2].blue()- p[2][3].blue()- .5 * p[3][0].blue()
//                + 1.25 * p[3][1].blue()- p[3][2].blue()+ .25 * p[3][3].blue());
//    a[2][3].setBlue( -.5 * p[0][0].blue()+ 1.5 * p[0][1].blue()- 1.5 * p[0][2].blue()+ .5 * p[0][3].blue()
//                + 1.25 * p[1][0].blue()- 3.75 * p[1][1].blue()+ 3.75 * p[1][2].blue()
//                - 1.25 * p[1][3].blue()- p[2][0].blue()+ 3 * p[2][1].blue()- 3 * p[2][2].blue()+ p[2][3].blue()
//                + .25 * p[3][0].blue()- .75 * p[3][1].blue()+ .75 * p[3][2].blue()- .25 * p[3][3].blue());
//    a[3][0].setBlue( -.5 * p[0][1].blue()+ 1.5 * p[1][1].blue()- 1.5 * p[2][1].blue()+ .5 * p[3][1].blue());
//    a[3][1].setBlue( .25 * p[0][0].blue()- .25 * p[0][2].blue()- .75 * p[1][0].blue()+ .75 * p[1][2].blue()
//                + .75 * p[2][0].blue()- .75 * p[2][2].blue()- .25 * p[3][0].blue()+ .25 * p[3][2].blue());
//    a[3][2].setBlue( -.5 * p[0][0].blue()+ 1.25 * p[0][1].blue()- p[0][2].blue()+ .25 * p[0][3].blue()
//                + 1.5 * p[1][0].blue()- 3.75 * p[1][1].blue()+ 3 * p[1][2].blue()- .75 * p[1][3].blue()
//                - 1.5 * p[2][0].blue()+ 3.75 * p[2][1].blue()- 3 * p[2][2].blue()+ .75 * p[2][3].blue()
//                + .5 * p[3][0].blue()- 1.25 * p[3][1].blue()+ p[3][2].blue()- .25 * p[3][3].blue());
//    a[3][3].setBlue( .25 * p[0][0].blue()- .75 * p[0][1].blue()+ .75 * p[0][2].blue()- .25 * p[0][3].blue()
//                - .75 * p[1][0].blue()+ 2.25 * p[1][1].blue()- 2.25 * p[1][2].blue()+ .75 * p[1][3].blue()
//                + .75 * p[2][0].blue()- 2.25 * p[2][1].blue()+ 2.25 * p[2][2].blue()- .75 * p[2][3].blue()
//                - .25 * p[3][0].blue()+ .75 * p[3][1].blue()- .75 * p[3][2].blue()+ .25 * p[3][3].blue());


}
RealPixel MainWindow::getValue(double x, double y)
{
//    //memorized powers to 2 and 3 .
//    double x2 = x * x;
//    double x3 = x2 * x;
//    double y2 = y * y;
//    double y3 = y2 * y;

//    return (a[0][0] + a[0][1] * y + a[0][2] * y2 + a[0][3] * y3) +
//           (a[1][0] + a[1][1] * y + a[1][2] * y2 + a[1][3] * y3) * x +
//           (a[2][0] + a[2][1] * y + a[2][2] * y2 + a[2][3] * y3) * x2 +
//           (a[3][0] + a[3][1] * y + a[3][2] * y2 + a[3][3] * y3) * x3;
    return p[int(x)][int(y)];
}
double k(double b, double c, double x)
{
    x = fabs(x);
    if(x<1)
        return (12-9*b-6*c)*x*x*x + (-18 + 12*b+6*c)*x*x + 6 - 2*b;
    else if(x >= 1 && x < 2)
        return (-b -6*c)*x*x*x + (6*b +30*c)*x*x + (-12*b -48*c)*x + 8*b + 24*c;
    else return 0;

}

RealPixel MainWindow::cubicInterpolate (RealPixel p[], double x) {
//    return p[1] + (p[2] - p[0] + (p[0]*2.0 - p[1]*5.0 + p[2]*4.0 - p[3] + (
//                                (p[1] - p[2])*3.0 + p[3] - p[0])*x)*x)*x*0.5;
//    p[1] + 0.5 * x*(p[2] - p[0] + x*(2.0*p[0] - 5.0*p[1] + 4.0*p[2] - p[3] + x*(3.0*(p[1] - p[2]) + p[3] - p[0])));
    return RealPixel(
            p[1].red() + 0.5 * x*(p[2].red() - p[0].red() + x*(2.0*p[0].red() - 5.0*p[1].red() +
            4.0*p[2].red() - p[3].red() + x*(3.0*(p[1].red() - p[2].red()) + p[3].red() - p[0].red()))),
            p[1].green() + 0.5 * x*(p[2].green() - p[0].green() + x*(2.0*p[0].green() - 5.0*p[1].green()
            + 4.0*p[2].green() - p[3].green() + x*(3.0*(p[1].green() - p[2].green()) + p[3].green() - p[0].green()))),
            p[1].blue() + 0.5 * x*(p[2].blue() - p[0].blue() + x*(2.0*p[0].blue() - 5.0*p[1].blue() +
            4.0*p[2].blue() - p[3].blue() + x*(3.0*(p[1].blue() - p[2].blue()) + p[3].blue() - p[0].blue()))));

    //return RealPixel(k(0,0.5,p[0].red()),k(0,0.5,p[0].green()),k(0,0.5,p[0].blue()));
      /*
       *
       * double d0 = p[0].red() - p[1].red(),
              d2 = p[2].red() - p[1].red(),
              d3 = p[3].red() - p[1].red(),
              a0 = p[1].red(),
              a1 = - (1/3.)*d0 + d2 - (1/6.)*d3,
              a2 = (1/2.)*d0 + (1/2.)*d2,
              a3 = -(1/6.)*d0 - (1/2.)*d2 + (1/6.)*d3,
              red = a0 + a1*x + a2*x*x + a3*x*x*x;

      double c0 = p[0].green() - p[1].green(),
                      c2 = p[2].green() - p[1].green(),
                      c3 = p[3].green() - p[1].green(),
                      m0 = p[1].green(),
                      m1 = - (1/3.)*c0 + c2 - (1/6.)*c3,
                      m2 = (1/2.)*c0 + (1/2.)*c2,
                      m3 = -(1/6.)*c0 - (1/2.)*c2 + (1/6.)*c3,
                      green = m0 + m1*x + m2*x*x + m3*x*x*x;
     double dd0 = p[0].blue() - p[1].blue(),
                              dd2 = p[2].blue() - p[1].blue(),
                              dd3 = p[3].blue() - p[1].blue(),
                              mm0 = p[1].blue(),
                              mm1 = - (1/3.)*dd0 + dd2 - (1/6.)*dd3,
                              mm2 = (1/2.)*dd0 + (1/2.)*dd2,
                              mm3 = -(1/6.)*dd0 - (1/2.)*dd2 + (1/6.)*dd3,
                              blue = mm0 + mm1*x + mm2*x*x + mm3*x*x*x;
                              return RealPixel(red,green,blue);*/

}

RealPixel MainWindow::bicubicInterpolate (//double p[4][4],
double x, double y) {
    RealPixel arr[4];
    arr[0] = cubicInterpolate(p[0], y);
    arr[1] = cubicInterpolate(p[1], y);
    arr[2] = cubicInterpolate(p[2], y);
    arr[3] = cubicInterpolate(p[3], y);
    return cubicInterpolate(arr, x);
}

void MainWindow::bicubic_interpolation()
{
    int i,j;
    //double p[4][4];

    for( i = 0; i<modifiedImageHeight; i++)
   //for (i = 1; i < imageHeight; i ++)
    {
       for ( j = 0; j < modifiedImageWidth; j++)//
        //for (j = 1; j < imageWidth; j ++)
        {

//            int x = (int)(j/zoom) +1 ;
//            int y = (int)(i/zoom) +1;
//            //double index = (y*imageWidth+x) ;
            //updateCoefficients();//Обновим коэффициенты
           double rzoom = 1./zoom;
           int x = (int)(rzoom * (j)) ;
           int y = (int)(rzoom * (i)) ;
           double x_diff = (rzoom * j) - x ;
           double y_diff = (rzoom * i) - y ;
           int index = (y*imageWidth+x);
            /*p[0][0] = imageMatrix[(i- 1)*imageWidth+(j- 1)];   // imageMatrix[0][0]
            p[1][0] = imageMatrix[(i)*imageWidth+(j- 1)];       // imageMatrix[1][0]
            p[2][0] = imageMatrix[(i+ 1)*imageWidth+(j- 1)];   // imageMatrix[2][0]
            p[3][0] = imageMatrix[(i+ 2)*imageWidth+(j- 1)];   // imageMatrix[3][0]

            p[0][1] = imageMatrix[(i- 1)*imageWidth+(j)];       // imageMatrix[0][1]
            p[1][1] = imageMatrix[(i)*imageWidth+(j)];           // imageMatrix[1][1]
            p[2][1] = imageMatrix[(i+ 1)*imageWidth+(j)];       // imageMatrix[2][1]
            p[3][1] = imageMatrix[(i+ 2)*imageWidth+(j)];       // imageMatrix[3][1]

            p[0][2] = imageMatrix[(i- 1)*imageWidth+(j+ 1)];   // imageMatrix[0][2]
            p[1][2] = imageMatrix[(i)*imageWidth+(j+ 1)];       // imageMatrix[1][2]
            p[2][2] = imageMatrix[(i+ 1)*imageWidth+(j+ 1)];   // imageMatrix[2][2]
            p[3][2] = imageMatrix[(i+ 2)*imageWidth+(j+ 1)];   // imageMatrix[3][2]

            p[0][3] = imageMatrix[(i- 1)*imageWidth+(j+ 2)];   // imageMatrix[0][3]
            p[1][3] = imageMatrix[(i)*imageWidth+(j+ 2)];       // imageMatrix[1][3]
            p[2][3] = imageMatrix[(i+ 1)*imageWidth+(j+ 2)];   // imageMatrix[2][3]
            p[3][3] = imageMatrix[(i+ 2)*imageWidth+(j+ 2)];*/   // imageMatrix[3][3]

           p[0][0] = imageMatrix[index];   // imageMatrix[0][0]
           p[1][0] = imageMatrix[index+imageWidth];       // imageMatrix[1][0]
           p[2][0] = imageMatrix[index+2*imageWidth];   // imageMatrix[2][0]
           p[3][0] = imageMatrix[index+3*imageWidth];   // imageMatrix[3][0]

           p[0][1] = imageMatrix[index+1];       // imageMatrix[0][1]
           p[1][1] = imageMatrix[index+imageWidth+1];          // imageMatrix[1][1]
           p[2][1] = imageMatrix[index+2*imageWidth+1];       // imageMatrix[2][1]
           p[3][1] = imageMatrix[index+3*imageWidth+1];       // imageMatrix[3][1]

           p[0][2] = imageMatrix[index+2];   // imageMatrix[0][2]
           p[1][2] = imageMatrix[index+imageWidth+2];       // imageMatrix[1][2]
           p[2][2] = imageMatrix[index+2*imageWidth+2];   // imageMatrix[2][2]
           p[3][2] = imageMatrix[index+3*imageWidth+2];   // imageMatrix[3][2]

           p[0][3] = imageMatrix[index+3];   // imageMatrix[0][3]
           p[1][3] = imageMatrix[index+imageWidth+3];       // imageMatrix[1][3]
           p[2][3] = imageMatrix[index+2*imageWidth+3];   // imageMatrix[2][3]
           p[3][3] = imageMatrix[index+3*imageWidth+3];   // imageMatrix[3][3]

//            double ax = j / zoom;
//            double ay = i / zoom;
//            modifiedMatrix[i*modifiedImageWidth + j] = bicubicInterpolate(ax,ay);
           double step = zoom;
             double x_res, y_res,inc = 1;
            for ( x_res = 0; x_res < step; x_res += inc) //Обход результирующего изображения
            {
                for ( y_res = 0; y_res < step; y_res += inc)
                {
                    int rx = int((i - 1) * step + x_res); //х пикселя на выходном изображении
                    int ry = int((j - 1) * step + y_res); //y пикселя на выходном изображении
                    double ax = x_res / step;             //x - x0
                    double ay = y_res / step;             //y - y0
                    //RealPixel val = getValue(ax, ay);//Получим значение
                    RealPixel val = bicubicInterpolate(ax,ay);//*1.0/(1+exp(-ay));

                    modifiedMatrix[i*modifiedImageWidth + j] = val; //bicubicInterpolate(ax,ay); //запишем точку
                }
            }
            // /*RealPixel val = bicubicInterpolate(x_diff,y_diff);
            // modifiedMatrix[i*modifiedImageWidth + j] = val;//*1.0/(1+exp(-x_diff));*/
            // end of interpolation//
        }
    }


}
/** END BICUBIC INTERPOLATION **/

void MainWindow::bilinear_interpolation()
{
    RealPixel a, b, c, d, *pixels = imageMatrix;
    int  x, y, index, h = imageHeight, w = imageWidth,
            h2 = modifiedImageHeight, w2 = modifiedImageWidth ;
    double x_ratio = ((double)(w))/w2 ;
    double y_ratio = ((double)(h))/h2 ;
    double x_diff, y_diff, blue, red, green ;
    int offset = 0 ;
    for (int i=0; i<h2; ++i) {
        for (int j=0; j<w2; ++j) {
            x = (int)(x_ratio * j) ;
            y = (int)(y_ratio * i) ;
            x_diff = (x_ratio * j) - x ;
            y_diff = (y_ratio * i) - y ;
            index = (y*w+x) ;
            a = pixels[index] ;
            b = pixels[index+1] ;
            c = pixels[index+w] ;
            d = pixels[index+w+1] ;

            // blue element
            // Yb = Ab(1-w)(1-h) + Bb(w)(1-h) + Cb(h)(1-w) + Db(wh)
            blue = (a.blue())*(1-x_diff)*(1-y_diff) + (b.blue())*(x_diff)*(1-y_diff) +
                   (c.blue())*(y_diff)*(1-x_diff)   + (d.blue())*(x_diff*y_diff);

            // green element
            // Yg = Ag(1-w)(1-h) + Bg(w)(1-h) + Cg(h)(1-w) + Dg(wh)
            green = (a.green())*(1-x_diff)*(1-y_diff) + (b.green())*(x_diff)*(1-y_diff) +
                    (c.green())*(y_diff)*(1-x_diff)   + (d.green())*(x_diff*y_diff);

            // red element
            // Yr = Ar(1-w)(1-h) + Br(w)(1-h) + Cr(h)(1-w) + Dr(wh)
            red = (a.red())*(1-x_diff)*(1-y_diff) + (b.red())*(x_diff)*(1-y_diff) +
                  (c.red())*(y_diff)*(1-x_diff)   + (d.red())*(x_diff*y_diff);

            modifiedMatrix[offset++] = RealPixel(red, green, blue);
        }
    }
}

void MainWindow::on_pushButton_clicked()
{
    setCursor(QCursor(Qt::WaitCursor));
    double t = clock();
    zoom = ui->coeff_resize->text().toDouble();
    zoom = fabs(zoom);
    modifiedImageWidth = (int)((double) imageWidth * zoom);
    modifiedImageHeight = (int)((double) imageHeight * zoom);

    delete[] modifiedMatrix;
    modifiedMatrix = new RealPixel[
        modifiedImageWidth * modifiedImageHeight
    ];
    bilinear_interpolation();
    computeModifiedImage(modifiedImageWidth,modifiedImageHeight,modifiedMatrix);
    drawArea->update();
    qDebug()<<"Biline x"<<zoom<<" time "<<(clock()-t)/CLOCKS_PER_SEC;
    setCursor(QCursor(Qt::ArrowCursor));
}

void MainWindow::on_pushButton_2_clicked()
{
    QCursor old = cursor();
    double t = clock();
    setCursor(QCursor(Qt::WaitCursor));
    zoom = ui->coeff_resize->text().toDouble();
    zoom = fabs(zoom);
    modifiedImageWidth = (int)((double) imageWidth * zoom);
    modifiedImageHeight = (int)((double) imageHeight * zoom);

    delete[] modifiedMatrix;

    modifiedMatrix = new RealPixel[modifiedImageWidth * modifiedImageHeight];
    bicubic_interpolation();
    //modifiedImageWidth -= 1; modifiedImageHeight -= 1;
    computeModifiedImage(modifiedImageWidth,modifiedImageHeight,modifiedMatrix);
    drawArea->update();
    qDebug()<<"Bicubic x"<<zoom<<" time "<<(clock()-t)/CLOCKS_PER_SEC;
    setCursor(old);

}

void MainWindow::on_HighPassBtn_clicked()
{

}

void MainWindow::on_highPassBtn_clicked()
{
    RealPixel *currMatrix;

    int currWidth, currHeigth;
    if (modifiedMatrix != 0)
        {
            currMatrix = modifiedMatrix;
            currWidth  = modifiedImageWidth;
            currHeigth = modifiedImageHeight;
        }
    else
        {
            currMatrix = imageMatrix;
            currWidth  = imageWidth;
            currHeigth = imageHeight;
        }
    RealPixel *newMatrix = new RealPixel[currWidth*currHeigth];
    for(int y = 1; y<currHeigth - 1; ++y)
    {
        for(int x = 1; x<currWidth - 1; ++x)
        {
            RealPixel p =
                (currMatrix[(y-1)*currWidth + x-1] +
                currMatrix[(y-1)*currWidth + x] +
                currMatrix[(y-1)*currWidth + x+1])*(-1.) +
                (currMatrix[(y+1)*currWidth + x-1] +
                currMatrix[(y+1)*currWidth + x] +
                currMatrix[(y+1)*currWidth + x+1])*(-1.) +
                (currMatrix[y*currWidth + x-1] +
                currMatrix[y*currWidth + x+1])*(-1.) +
                currMatrix[y*currWidth + x]*9.;

            p.sigma((double)ui->sigmaSlider->value());
            newMatrix[y*currWidth + x] = p;
        }
    }

    computeModifiedImage(currWidth, currHeigth, newMatrix);
    drawArea->update();

}

void MainWindow::on_splineButton_clicked()
{
    setCursor(QCursor(Qt::WaitCursor));
    double t = clock();
    zoom = ui->coeff_resize->text().toDouble();
    zoom = fabs(zoom);
    modifiedImageWidth = (int)((double) imageWidth * zoom);
    modifiedImageHeight = (int)((double) imageHeight * zoom);
    double zoomX, zoomY;
    delete[] modifiedMatrix;
    splineInterpolation(
        imageWidth, imageHeight,
        imageMatrix,
        zoom,
        zoomX, zoomY,
        modifiedImageWidth, modifiedImageHeight,
        &modifiedMatrix,
        splineType
    );
    computeModifiedImage(modifiedImageWidth,modifiedImageHeight,modifiedMatrix);
    drawArea->update();
    qDebug()<<"Spline "<<(splineType==1?"C1":"C2")<<" x"<<zoom<<" time "<<(clock()-t)/CLOCKS_PER_SEC;
    setCursor(QCursor(Qt::ArrowCursor));
}

void MainWindow::on_radioC1Spline_clicked()
{
    splineType = 1;
}

void MainWindow::on_radioC2Spline_clicked()
{
    splineType = 0;
}

void MainWindow::on_saveToClip_clicked()
{
    QClipboard *clip = QGuiApplication::clipboard();
    clip->setImage(*modifiedImage);
}
