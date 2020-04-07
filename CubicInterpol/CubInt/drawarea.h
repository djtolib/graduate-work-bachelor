#ifndef DRAWAREA_H
#define DRAWAREA_H

#include <QWidget>
#include <QColor>
#include "R2Graph.h"
#include "../cubint.h"

const int MAX_POINTS = 1024;

// Radio button value
const int DRAW_POLYNOMIAL = 0;
const int DRAW_C1_SPLINE = 1;
const int DRAW_C2_SPLINE = 2;

class DrawArea: public QWidget
{
    Q_OBJECT
private:
    QColor bgColor; // Background color
    int xSize;      // Window width
    int ySize;      // Window height

    // Mathematical coordinate system in window
    double xmin, xmax, ymin, ymax;
    double xCoeff, yCoeff;

    // Correspondence between pixel and mathematical coord. systems
    QPoint map(const R2Point& p) const;
    R2Point invMap(const QPoint& q) const;

    QPointF mapF(const R2Point& p) const;
    R2Point invMap(const QPointF& q) const;

    // The array of points defined by mouse clicks
    R2Point points[MAX_POINTS];
    Qt::MouseButton pointButtons[MAX_POINTS];
    int numPoints;

    void sortPoints();

public:
    CubicPolynomial cubicPol;
    CubicSpline cubicSpline;
    bool c1SplineComputed;
    bool c2SplineComputed;
    int whatToDraw; // DRAW_POLYNOMIAL / DRAW_C1_SPLINE / DRAW_C2_SPLINE

public:
    DrawArea(QWidget *parent = 0);

    double getXMin() const { return xmin; }
    double getXMax() const { return xmax; }
    double getYMin() const { return ymin; }
    double getYMax() const { return ymax; }

    void setCoordinates(
        double xMin, double xMax,
        double yMin, double yMax
    );
    void initializeMap();

    void drawCoordSystem(QPainter*);
    void drawPoints(QPainter*);
    void plotFunction(QPainter*);
    void interpolate();
    void computeC1Spline();
    void computeC2Spline();
    void drawSpline(QPainter*);
    void clear();
    void onDraw();  // Called when a user presses the "Draw" button

    // Function to plot
    double f(double x) const;

protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent* event);
    void resizeEvent(QResizeEvent *event);
};

#endif // DRAWAREA_H
