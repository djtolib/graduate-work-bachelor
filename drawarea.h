#ifndef DRAWAREA_H
#define DRAWAREA_H

#include <QWidget>
#include <QColor>
#include <QImage>
#include "RealPixel.h"

class DrawArea: public QWidget
{
    Q_OBJECT
public:
    QColor bgColor;
    int xSize;
    int ySize;

    DrawArea(QWidget *parent = 0);


protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
};

#endif // DRAWAREA_H
