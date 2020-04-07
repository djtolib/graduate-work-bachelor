#include "RealPixel.h"
#include "CubicInterpol/cubint.h"

static inline double restrict01(double v) {
    if (v < 0.)
        return 0.;
    else if (v > 1.)
        return 1.;
    else
        return v;
}

void splineInterpolation(
    int imageWidth, int imageHeight,
    const RealPixel* imageMatrix,
    double zoom,
    double& realZoomX, double& realZoomY,
    int& zoomedWidth, int& zoomedHeight,
    RealPixel** zoomedMatrix,
    int splineType /* = 0 */    // 0 -- C2-cubic spline, 1 -- C1-spline
) {
    zoomedWidth = (int)(imageWidth*zoom + 0.49);
    zoomedHeight = (int)(imageHeight*zoom + 0.49);
    realZoomX = (double) zoomedWidth / (double) imageWidth;
    realZoomY = (double) zoomedHeight / (double) imageHeight;

    RealPixel* zoomedMatrX = new RealPixel[zoomedWidth*imageHeight];
    CubicSpline redSpline(imageWidth);
    CubicSpline greenSpline(imageWidth);
    CubicSpline blueSpline(imageWidth);
    double dx = realZoomX;
    for (int y = 0; y < imageHeight; ++y) {
        int nodeIdx;
        double nodeX = 0.;
        for (nodeIdx = 0; nodeIdx < imageWidth; ++nodeIdx) {
            redSpline.nodes[nodeIdx].x = nodeX;
            greenSpline.nodes[nodeIdx].x = nodeX;
            blueSpline.nodes[nodeIdx].x = nodeX;

            RealPixel p = imageMatrix[y*imageWidth + nodeIdx];
            redSpline.nodes[nodeIdx].y = p.red();
            greenSpline.nodes[nodeIdx].y = p.green();
            blueSpline.nodes[nodeIdx].y = p.blue();
            nodeX += dx;
        }
        if (splineType == 0) {
            redSpline.interpolateC2();
            greenSpline.interpolateC2();
            blueSpline.interpolateC2();
        } else {
            redSpline.interpolateC1();
            greenSpline.interpolateC1();
            blueSpline.interpolateC1();
        }

        nodeIdx = 0;
        for (int x = 0; x < zoomedWidth; ++x) {
            double xx = (double) x;
            while (
                nodeIdx < imageWidth-2 &&
                xx >= redSpline.nodes[nodeIdx+1].x
            )
                ++nodeIdx;

            double v = redSpline.value(
                xx, nodeIdx
            );
            zoomedMatrX[y*zoomedWidth + x].setRed(v);
            v = greenSpline.value(
                xx, nodeIdx
            );
            zoomedMatrX[y*zoomedWidth + x].setGreen(v);
            v = blueSpline.value(
                xx, nodeIdx
            );
            zoomedMatrX[y*zoomedWidth + x].setBlue(v);
        }
    } // end for (y...

    RealPixel* zoomedMatrY = new RealPixel[zoomedWidth*zoomedHeight];
    redSpline.resize(imageHeight);
    greenSpline.resize(imageHeight);
    blueSpline.resize(imageHeight);
    double dy = realZoomY;
    for (int x = 0; x < zoomedWidth; ++x) {
        int nodeIdx;
        double nodeY = 0.;
        for (nodeIdx = 0; nodeIdx < imageHeight; ++nodeIdx) {
            redSpline.nodes[nodeIdx].x = nodeY;
            greenSpline.nodes[nodeIdx].x = nodeY;
            blueSpline.nodes[nodeIdx].x = nodeY;

            RealPixel p = zoomedMatrX[nodeIdx*zoomedWidth + x];
            redSpline.nodes[nodeIdx].y = p.red();
            greenSpline.nodes[nodeIdx].y = p.green();
            blueSpline.nodes[nodeIdx].y = p.blue();
            nodeY += dy;
        }
        if (splineType == 0) {
            redSpline.interpolateC2();
            greenSpline.interpolateC2();
            blueSpline.interpolateC2();
        } else {
            redSpline.interpolateC1();
            greenSpline.interpolateC1();
            blueSpline.interpolateC1();
        }

        nodeIdx = 0;
        for (int y = 0; y < zoomedHeight; ++y) {
            double yy = (double) y;
            while (
                nodeIdx < imageHeight-2 &&
                yy >= redSpline.nodes[nodeIdx+1].x
            )
                ++nodeIdx;

            double v = redSpline.value(
                yy, nodeIdx
            );
            zoomedMatrY[y*zoomedWidth + x].setRed(restrict01(v));
            v = greenSpline.value(
                yy, nodeIdx
            );
            zoomedMatrY[y*zoomedWidth + x].setGreen(restrict01(v));
            v = blueSpline.value(
                yy, nodeIdx
            );
            zoomedMatrY[y*zoomedWidth + x].setBlue(restrict01(v));
        }
    } // end for (x...

    delete[] zoomedMatrX;
    *zoomedMatrix = zoomedMatrY;
}
