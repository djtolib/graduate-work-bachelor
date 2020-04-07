#include <QtGui>
#include <vector>
#include <algorithm>
#include "drawarea.h"
#include "mainwindow.h"
#include "../cubint.h"

DrawArea::DrawArea(QWidget *parent):
    QWidget(parent),
    bgColor(Qt::lightGray),
    xSize(0),
    ySize(0),
    xmin(-8.),
    xmax(8.),
    ymin(-6.),
    ymax(6.),
    xCoeff(50.),        // == 800./(xmax-xmin)
    yCoeff(50.),        // == 600./(ymax-ymin)
    numPoints(0),
    cubicPol(),
    cubicSpline(),
    c1SplineComputed(false),
    c2SplineComputed(false),
    whatToDraw(DRAW_POLYNOMIAL)
{
    setAttribute(Qt::WA_StaticContents); // for optimizing painting events
    drawArea = this;
}

void DrawArea::paintEvent(QPaintEvent* /* event */)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(0, 0, width(), height(), bgColor);

    drawCoordSystem(&painter);
    if (whatToDraw == DRAW_POLYNOMIAL) {
        if (numPoints >= 4) {
            plotFunction(&painter);

            QPen redPen(Qt::red);
            painter.setPen(redPen);
            R2Vector v = points[1] - points[0];
            if (v.length() < 3.)
                v *= 3./v.length();
            QPointF p0 = mapF(points[0] - v*0.5);
            QPointF p1 = mapF(points[1] + v*1.5);
            painter.drawLine(p0, p1);

            v = points[2] - points[3];
            if (v.length() < 3.)
                v *= 3./v.length();
            p0 = mapF(points[3] - v*0.5);
            p1 = mapF(points[3] + v*1.5);
            painter.drawLine(p0, p1);
        }
    } else if (
        whatToDraw == DRAW_C1_SPLINE ||
        whatToDraw == DRAW_C2_SPLINE
    ) {
        drawSpline(&painter);
    }
    drawPoints(&painter);
}

void DrawArea::resizeEvent(QResizeEvent* /* event */) {
    int w = width();
    int h = height();
    if (
        w != 0 && h != 0 && (
            xSize == 0 || ySize == 0 ||
            xSize != width() || ySize != height()
        )
    ) {
        xSize = w;
        ySize = h;
        initializeMap();
        mainWindow->showCoordRange(
            xmin, xmax, ymin, ymax
        );
    }
}

void DrawArea::initializeMap() {
    int w = width();
    if (w == 0)
        return;

    int h = height();
    if (h == 0)
        return;

    // We try to save the relative position of center
    double yCenter = (0. - ymin)/(ymax - ymin);

    double aspect = (double) h / (double) w;
    double dx = xmax - xmin;
    double dy = dx*aspect;
    ymin = 0. - dy*yCenter;
    ymax = ymin + dy;

    xCoeff = (double) w / dx;
    yCoeff = (double) h / dy;
}

QPoint DrawArea::map(const R2Point& p) const {
    return QPoint(
        floor((p.x - xmin)*xCoeff + 0.5),
        floor((ymax - p.y)*yCoeff + 0.5)
    );
}

QPointF DrawArea::mapF(const R2Point& p) const {
    return QPoint(
        (p.x - xmin)*xCoeff,
        (ymax - p.y)*yCoeff
    );
}

R2Point DrawArea::invMap(const QPointF& p) const {
    return R2Point(
        xmin + (p.x()/xCoeff),
        ymax - (p.y()/yCoeff)
    );
}

R2Point DrawArea::invMap(const QPoint& p) const {
    return R2Point(
        xmin + (p.x()/xCoeff),
        ymax - (p.y()/yCoeff)
    );
}

void DrawArea::setCoordinates(
    double xMin, double xMax,
    double yMin, double yMax
) {
    if (xMax <= xMin || yMax <= yMin)   // Incorrect parameters
        return;
    xmin = xMin; xmax = xMax;
    ymin = yMin; ymax = yMax;
    initializeMap();
    update();
}

void DrawArea::onDraw() {
    update();   // Redraw the picture in window
}

void DrawArea::drawCoordSystem(QPainter* qp) {
    int w = width();
    int h = height();
    if (w == 0 || h == 0)
        return;

    QColor lightGray(220, 220, 220);
    QColor gray(128, 128, 128);
    QColor black(0, 0, 0);

    qp->setBrush(QBrush(lightGray));
    qp->drawRect(0, 0, w, h);

    qp->setPen(gray);

    QPoint p0, p1;
    int ix = (int) xmin;
    while (ix <= (int) xmax) {
        if (ix == 0) {
            ++ix;
            continue;
        }
        p0 = map(R2Point(ix, ymin));
        p1 = map(R2Point(ix, ymax));
        qp->drawLine(p0, p1);
        ++ix;
    }

    int iy = (int) ymin;
    while (iy <= (int) ymax) {
        if (iy == 0) {
            ++iy;
            continue;
        }
        p0 = map(R2Point(xmin, iy));
        p1 = map(R2Point(xmax, iy));
        qp->drawLine(p0, p1);
        ++iy;
    }

    qp->setPen(black);
    p0 = map(R2Point(xmin, 0.));
    p1 = map(R2Point(xmax, 0.));
    qp->drawLine(p0, p1);

    p0 = map(R2Point(0., ymin));
    p1 = map(R2Point(0., ymax));
    qp->drawLine(p0, p1);
}

void DrawArea::mousePressEvent(QMouseEvent* event) {
    if (whatToDraw == DRAW_POLYNOMIAL && numPoints >= 4) {
        numPoints = 0;
    }

    pointButtons[numPoints] = event->button();
    R2Point p = invMap(event->pos());
    points[numPoints] = p;
    ++numPoints;
    sortPoints();
    if (whatToDraw == DRAW_POLYNOMIAL) {
        if (numPoints >= 4) {
            interpolate();
        }
        c1SplineComputed = false;
        c2SplineComputed = false;
    } else if (whatToDraw == DRAW_C1_SPLINE) {
        computeC1Spline();
        c2SplineComputed = false;
    } else if (whatToDraw == DRAW_C2_SPLINE) {
        computeC2Spline();
        c1SplineComputed = false;
    }

    update();
}

void DrawArea::drawPoints(QPainter* painter) {
    QPen bluePen(Qt::blue);
    bluePen.setWidth(2);

    QPen redPen(Qt::red);
    redPen.setWidth(2);

    QPen magentaPen(Qt::magenta);
    magentaPen.setWidth(2);

    QPoint dx(8, 0);
    QPoint dy(0, 8);
    for (int i = 0; i < numPoints; ++i) {
        if (pointButtons[i] == Qt::LeftButton)
            painter->setPen(bluePen);
        else if (pointButtons[i] == Qt::RightButton)
            painter->setPen(redPen);
        else // Qt::MiddleButton
            painter->setPen(magentaPen);

        QPoint t = map(points[i]);
        painter->drawLine(t-dx, t+dx);
        painter->drawLine(t-dy, t+dy);
    }
}

void DrawArea::plotFunction(QPainter* painter) {
    QPen greenPen(Qt::darkGreen);
    greenPen.setWidth(2);
    painter->setPen(greenPen);

    // double dx = 0.01;
    // Compute dx that corresponds to 1 pixel
    R2Point h0, h1;
    h0 = invMap(QPoint(0, 0));
    h1 = invMap(QPoint(1, 0));
    double dx = h1.x - h0.x;

    R2Point p0(xmin, f(xmin));
    R2Point p1 = p0;
    while (p1.x <= xmax) {
        p0 = p1;
        p1.x += dx;
        p1.y = f(p1.x);
        painter->drawLine(
            map(p0), map(p1)
        );
    }
}

void DrawArea::drawSpline(QPainter* painter) {
    if (
        (!c1SplineComputed && !c2SplineComputed) ||
        numPoints <= 0
    )
        return;
    QPen greenPen(Qt::darkGreen);
    greenPen.setWidth(2);
    painter->setPen(greenPen);

    // double dx = 0.01;
    // Compute dx that corresponds to 1 pixel
    R2Point h0, h1;
    h0 = invMap(QPoint(0, 0));
    h1 = invMap(QPoint(1, 0));
    double dx = h1.x - h0.x;

    int idx = 0;
    double minX = cubicSpline.nodes[idx].x;
    double maxX = cubicSpline.nodes[numPoints-1].x;

    R2Point p0(minX, cubicSpline.value(minX, idx));
    R2Point p1 = p0;
    while (p1.x <= maxX) {
        p0 = p1;
        p1.x += dx;
        if (idx < numPoints - 2 && p1.x >= cubicSpline.nodes[idx + 1].x) {
            ++idx;
        }
        p1.y = cubicSpline.value(p1.x, idx);
        painter->drawLine(
            map(p0), map(p1)
        );
    }
}

void DrawArea::clear() {
    numPoints = 0;
    c1SplineComputed = false;
    c2SplineComputed = false;

    update();
}

double DrawArea::f(double x) const {
    return cubicPol.value(x);
}

void DrawArea::computeC1Spline() {
    cubicSpline.resize(numPoints);
    for (int i = 0; i < numPoints; ++i)
        cubicSpline.nodes[i] = points[i];
    if (numPoints >= 2) {
        cubicSpline.interpolateC1();
    }
    c1SplineComputed = true;
}

void DrawArea::computeC2Spline() {
    cubicSpline.resize(numPoints);
    for (int i = 0; i < numPoints; ++i)
        cubicSpline.nodes[i] = points[i];
    if (numPoints >= 2) {
        cubicSpline.interpolateC2();
    }
    c2SplineComputed = true;
}

void DrawArea::sortPoints() {
    if (numPoints <= 1)
        return;
    std::vector<R2Point> nodes;
    nodes.resize(numPoints);
    for (int i = 0; i < numPoints; ++i)
        nodes[i] = points[i];
    std::sort(nodes.begin(), nodes.end());
    for (int i = 0; i < numPoints; ++i)
        points[i] = nodes[i];
}

void DrawArea::interpolate() {
    if (numPoints >= 4) {
        cubicPol.interpolate(
            points[0].x, points[3].x,
            points[0].y,
            (points[1].y - points[0].y)/(points[1].x - points[0].x),
            points[3].y,
            (points[3].y - points[2].y)/(points[3].x - points[2].x)
        );
    }
    c1SplineComputed = false;
    c2SplineComputed = false;
}
