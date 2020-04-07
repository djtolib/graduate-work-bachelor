#include <QtGui>
#include "drawarea.h"
#include "mainwindow.h"

DrawArea::DrawArea(QWidget *parent) :
    QWidget(parent),
    bgColor(Qt::darkGray),
    xSize(0),
    ySize(0)
{
    setAttribute(Qt::WA_StaticContents); // for optimizing painting events
    drawArea = this;
}

void DrawArea::paintEvent(QPaintEvent* /* event */)
{
    if (
        xSize == 0 || ySize == 0 ||
        xSize != width() || ySize != height()
    ) {
        xSize = width();
        ySize = height();
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(0, 0, width(), height(), bgColor);

    if (mainWindow->modifiedImage != 0)
        painter.drawImage(0, 0, *(mainWindow->modifiedImage));
    else if (mainWindow->image != 0)
        painter.drawImage(0, 0, *(mainWindow->image));
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
        //... to do...

    }
}
