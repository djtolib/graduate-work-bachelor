#include <cassert>
#include <cstdio>
#include "cubint.h"
#include "bandmatrix.h"

// Interpolate so that
//     p(a) = value0, dp(a) = derivative0
//     p(b) = value0, dp(b) = derivative1
//
CubicPolynomial& CubicPolynomial::interpolate(
    double a, double b,
    double va, double da,
    double vb, double db
) {
    double a2 = a*a; double a3 = a2*a;
    double b2 = b*b; // double b3 = b2*b;
    double a_b = a - b;
    double a_b2 = a_b*a_b;
    double a_b3 = a_b2*a_b;
    // coeff[0] =
    // -a^3*b*da/(a^3 - 3*a^2*b + 3*a*b^2 - b^3)
    // + a^2*b^2*da/(a^3 - 3*a^2*b + 3*a*b^2 - b^3)
    // - a^3*b*db/(a^3 - 3*a^2*b + 3*a*b^2 - b^3)
    // + a^2*b^2*db/(a^3 - 3*a^2*b + 3*a*b^2 - b^3)
    // + a^2*b*da/(a^2 - 2*a*b + b^2)
    // - a*b^2*da/(a^2 - 2*a*b + b^2)
    // + 2*a^2*b*va/(a^3 - 3*a^2*b + 3*a*b^2 - b^3)
    // - 2*a^2*b*vb/(a^3 - 3*a^2*b + 3*a*b^2 - b^3)
    // - a*b*va/(a^2 - 2*a*b + b^2)
    // + a*b*vb/(a^2 - 2*a*b + b^2)
    // - a*va/(a - b)
    // + va
    // + a*vb/(a - b)
    coeff[0] =
        (-a3*b*da + a2*b2*da - a3*b*db+ a2*b2*db
            + 2*a2*b*va- 2*a2*b*vb)/a_b3 +
        (a2*b*da - a*b2*da - a*b*va + a*b*vb)/a_b2 +
        (-a*va + a*vb)/a_b +
        va;

    // coeff[1] =
    // a^3*da/(a^3 - 3*a^2*b + 3*a*b^2 - b^3)
    // + a^2*b*da/(a^3 - 3*a^2*b + 3*a*b^2 - b^3)
    // - 2*a*b^2*da/(a^3 - 3*a^2*b + 3*a*b^2 - b^3)
    // + a^3*db/(a^3 - 3*a^2*b + 3*a*b^2 - b^3)
    // + a^2*b*db/(a^3 - 3*a^2*b + 3*a*b^2 - b^3)
    // - 2*a*b^2*db/(a^3 - 3*a^2*b + 3*a*b^2 - b^3)
    // - a^2*da/(a^2 - 2*a*b + b^2)
    // + b^2*da/(a^2 - 2*a*b + b^2)
    // - 2*a^2*va/(a^3 - 3*a^2*b + 3*a*b^2 - b^3)
    // - 4*a*b*va/(a^3 - 3*a^2*b + 3*a*b^2 - b^3)
    // + 2*a^2*vb/(a^3 - 3*a^2*b + 3*a*b^2 - b^3)
    // + 4*a*b*vb/(a^3 - 3*a^2*b + 3*a*b^2 - b^3)
    // + a*va/(a^2 - 2*a*b + b^2)
    // + b*va/(a^2 - 2*a*b + b^2)
    // - a*vb/(a^2 - 2*a*b + b^2)
    // - b*vb/(a^2 - 2*a*b + b^2)
    // + va/(a - b)
    // - vb/(a - b)
    coeff[1] =
        (a3*da + a2*b*da - 2*a*b2*da + a3*db + a2*b*db - 2*a*b2*db
            - 2*a2*va - 4*a*b*va + 2*a2*vb + 4*a*b*vb)/a_b3 +
        (-a2*da + b2*da + a*va + b*va - a*vb - b*vb)/a_b2 +
        (va - vb)/a_b;

    // coeff[2] =
    // - 2*a^2*da/(a^3 - 3*a^2*b + 3*a*b^2 - b^3)
    // + a*b*da/(a^3 - 3*a^2*b + 3*a*b^2 - b^3)
    // + b^2*da/(a^3 - 3*a^2*b + 3*a*b^2 - b^3)
    // - 2*a^2*db/(a^3 - 3*a^2*b + 3*a*b^2 - b^3)
    // + a*b*db/(a^3 - 3*a^2*b + 3*a*b^2 - b^3)
    // + b^2*db/(a^3 - 3*a^2*b + 3*a*b^2 - b^3)
    // + a*da/(a^2 - 2*a*b + b^2)
    // - b*da/(a^2 - 2*a*b + b^2)
    // + 4*a*va/(a^3 - 3*a^2*b + 3*a*b^2 - b^3)
    // + 2*b*va/(a^3 - 3*a^2*b + 3*a*b^2 - b^3)
    // - 4*a*vb/(a^3 - 3*a^2*b + 3*a*b^2 - b^3)
    // - 2*b*vb/(a^3 - 3*a^2*b + 3*a*b^2 - b^3)
    // - va/(a^2 - 2*a*b + b^2)
    // + vb/(a^2 - 2*a*b + b^2)
    coeff[2] =
        (-2*a2*da + a*b*da + b2*da - 2*a2*db + a*b*db + b2*db
            + 4*a*va + 2*b*va - 4*a*vb - 2*b*vb)/a_b3 +
        (a*da - b*da - va + vb)/a_b2;

    // coeff[3] =
    // + a*da/(a^3 - 3*a^2*b + 3*a*b^2 - b^3)
    // - b*da/(a^3 - 3*a^2*b + 3*a*b^2 - b^3)
    // + a*db/(a^3 - 3*a^2*b + 3*a*b^2 - b^3)
    // - b*db/(a^3 - 3*a^2*b + 3*a*b^2 - b^3)
    // - 2*va/(a^3 - 3*a^2*b + 3*a*b^2 - b^3)
    // + 2*vb/(a^3 - 3*a^2*b + 3*a*b^2 - b^3)
    coeff[3] =
        // (a*da - b*da + a*db - b*db - 2*va + 2*vb)/a_b3
        (a_b*(da + db) - 2*(va - vb))/a_b3;
    return *this;
}

int CubicSpline::findSegment(double x) const {  // Binary search
    assert(numNodes >= 2);
    if (numNodes < 2)
        return (-1);

    if (x < nodes[0].x) {
        return (-1);
    } else if (x >= nodes[numNodes - 1].x) {
        return numNodes - 1;
    } else {
        assert(nodes[0].x <= x && x < nodes[numNodes - 1].x);
        int a = 0; int b = numNodes - 1;
        while (b - a > 1) {
            int c = (a + b)/2;
            if (x < nodes[c].x) {
                b = c;
            } else {
                a = c;
            }
        }

        assert(nodes[a].x <= x && x < nodes[a + 1].x);
        return a;
    }
}

// Value of the spline
// If nodeIdx >= 0, then x belongs
// to the interval nodes[nodeIdx].x <= x <= nodes[nodeIdx+1].x
double CubicSpline::value(double x, int nodeIdx /* = (-1) */) const {
    assert(numNodes > 0);
    if (numNodes == 1)
        return nodes[0].y;
    int idx = nodeIdx;
    if (idx < 0) {
        idx = findSegment(x);
        if (idx < 0)
            idx = 0;
        else if (idx >= numNodes)
            idx = numNodes - 1;
    }

    assert(0 <= idx && idx < numNodes - 1);
    return polynomials[idx].value(x);
}

// Calculate cubic polynomials so the the spline will be C1-continues
CubicSpline& CubicSpline::interpolateC1() {
    assert(numNodes > 1);
    if (numNodes <= 1)
        return *this;
    if (directions == 0)
        directions = new R2Vector[numNodes];

    for (int i = 1; i < numNodes - 1; ++i) {
        R2Vector v0 = nodes[i] - nodes[i-1];
        v0.normalize();
        R2Vector v1 = nodes[i+1] - nodes[i];
        v1.normalize();
        directions[i] = v0 + v1;
        //... directions[i].normalize();
    }

    if (numNodes == 2) {
        R2Vector v = nodes[1] - nodes[0];
        directions[0] = v;
        directions[1] = v;
    } else {
        assert(numNodes >= 3);
        double dx = (nodes[1].x - nodes[0].x)*0.5;
        R2Point p = R2Point(
            nodes[1].x - dx,
            nodes[1].y - dx*(directions[1].y/directions[1].x)
        );
        directions[0] = p - nodes[0];
        //... directions[0].normalize();

        dx = (nodes[numNodes-1].x - nodes[numNodes-2].x)*0.5;
        p = R2Point(
            nodes[numNodes-2].x + dx,
            nodes[numNodes-2].y +
                dx*(directions[numNodes-2].y/directions[numNodes-2].x)
        );
        directions[numNodes-1] = nodes[numNodes-1] - p;
        //... directions[numNodes-1].normalize();
    }
    for (int i = 0; i < numNodes - 1; ++i) {
        polynomials[i].interpolate(
            nodes[i].x, nodes[i+1].x,
            nodes[i].y, directions[i].y / directions[i].x,
            nodes[i+1].y, directions[i+1].y / directions[i+1].x
        );
    }
    return *this;
}

void CubicSpline::resize(int n) {
    if (n == numNodes)
        return;

    if (n > numNodes) {
        R2Point* newNodes = new R2Point[n];
        for (int i = 0; i < numNodes; ++i)
             newNodes[i] = nodes[i];
        delete[] nodes;
        delete[] polynomials;
        delete[] directions;

        polynomials = new CubicPolynomial[n];
        directions = new R2Vector[n];
        nodes = newNodes;

        delete bandMatrix; bandMatrix = 0;
        delete[] coeffs; coeffs = 0;
    } else {
        if (bandMatrix != 0)
            bandMatrix->resize((n - 1)*4, 5, 4);
    }
    numNodes = n;
}

// Calculate cubic polynomials so the the spline will be C2-continues,
// solving a system of linear equarions with band 9-diagonal matrix
CubicSpline& CubicSpline::interpolateC2() {
    if (numNodes <= 1)
        return *this;

    int n = (numNodes - 1)*4;
    //... BandExtMatrix matr(n, 5, 4);
    if (bandMatrix == 0) {
        bandMatrix = new BandExtMatrix(n, 5, 4);
    }
    if (coeffs == 0) {
        coeffs = new double[n];
    }
    bandMatrix->init();

    // Fill in the matrix of linear system
    int i = 0, j0 = 0;
    int nodeIdx = 0;
    double nodeX = nodes[nodeIdx].x;
    double nodeX2 = nodeX*nodeX;
    double nodeX3 = nodeX2*nodeX;
    double nodeY = nodes[nodeIdx].y;

    // 1. First node: value == nodeY
    bandMatrix->at(i, 0) = 1.;
    bandMatrix->at(i, 1) = nodeX;
    bandMatrix->at(i, 2) = nodeX2;
    bandMatrix->at(i, 3) = nodeX3;
    bandMatrix->right(i) = nodeY;
    ++i;

    // 2. First node: second derivative == 0
    bandMatrix->at(i, 2) = 2.;
    bandMatrix->at(i, 3) = 6.*nodeX;
    ++i;

    ++nodeIdx;
    nodeX = nodes[nodeIdx].x;
    nodeX2 = nodeX*nodeX;
    nodeX3 = nodeX2*nodeX;
    nodeY = nodes[nodeIdx].y;

    // 2. Intermediate nodes
    while (nodeIdx < numNodes - 1) {

        // 1. Value of (nodeIdx-1)-th curve in node nodeIdx
        j0 = nodeIdx*4;
        bandMatrix->at(i, (j0 - 4)) = 1.;
        bandMatrix->at(i, (j0 - 4) + 1) = nodeX;
        bandMatrix->at(i, (j0 - 4) + 2) = nodeX2;
        bandMatrix->at(i, (j0 - 4) + 3) = nodeX3;
        bandMatrix->right(i) = nodeY;
        ++i;

        // 2. Value of nodeIdx-th curve in node nodeIdx
        bandMatrix->at(i, j0) = 1.;
        bandMatrix->at(i, j0 + 1) = nodeX;
        bandMatrix->at(i, j0 + 2) = nodeX2;
        bandMatrix->at(i, j0 + 3) = nodeX3;
        bandMatrix->right(i) = nodeY;
        ++i;

        // 3. Values of derivatives of 2 adjacent curves in nodeIdx
        // are the same
        // f   = c0 + c1*x + c2*x^2 + c3*x^3
        // f'  =      c1   + 2*c2*x + 3*c3*x^2
        // f'' =             2*c2   + 6*c3*x
        bandMatrix->at(i, (j0-4) + 1) = 1.;            // c1
        bandMatrix->at(i, (j0-4) + 2) = 2.*nodeX;      // c2
        bandMatrix->at(i, (j0-4) + 3) = 3.*nodeX2;     // c3

        bandMatrix->at(i, j0 + 1) = (-1.);             // c1
        bandMatrix->at(i, j0 + 2) = (-2.*nodeX);       // c2
        bandMatrix->at(i, j0 + 3) = (-3.*nodeX2);      // c3
        ++i;

        // 4. Values of derivatives of 2 adjacent curves in nodeIdx
        // are the same
        bandMatrix->at(i, (j0-4) + 2) = 2.;            // c2
        bandMatrix->at(i, (j0-4) + 3) = 6.*nodeX;      // c3

        bandMatrix->at(i, j0 + 2) = (-2.);             // c2
        bandMatrix->at(i, j0 + 3) = (-6.*nodeX);       // c3
        ++i;

        ++nodeIdx;      // Go to the next node
        nodeX = nodes[nodeIdx].x;
        nodeX2 = nodeX*nodeX;
        nodeX3 = nodeX2*nodeX;
        nodeY = nodes[nodeIdx].y;

    } // end while

    // 1. Last node: value == nodeY
    assert(nodeIdx == numNodes - 1);

    j0 = nodeIdx*4 - 4;
    assert(j0 == n-4);

    bandMatrix->at(i, j0) = 1.;
    bandMatrix->at(i, j0 + 1) = nodeX;
    bandMatrix->at(i, j0 + 2) = nodeX2;
    bandMatrix->at(i, j0 + 3) = nodeX3;
    bandMatrix->right(i) = nodeY;
    ++i;

    // 2. Last node: second derivative == 0
    bandMatrix->at(i, j0 + 2) = 2.;
    bandMatrix->at(i, j0 + 3) = 6.*nodeX;

    if (coeffs == 0) {
        coeffs = new double[n];
    }

#   ifndef NDEBUG
    BandExtMatrix initialMatrix = *bandMatrix;
    bool res =
#   endif

    bandMatrix->solve(coeffs);
    //... assert(res);

#   ifndef NDEBUG
    if (!res) {
        printf("Solve failed...\n");
        printf("InitialMatrix:\n");
        initialMatrix.print();
        printf("After gauss:\n");
        bandMatrix->print();
    }
#   endif

    i = 0;
    j0 = 0;
    while (i < numNodes - 1) {
        polynomials[i].coeff[0] = coeffs[j0];
        polynomials[i].coeff[1] = coeffs[j0 + 1];
        polynomials[i].coeff[2] = coeffs[j0 + 2];
        polynomials[i].coeff[3] = coeffs[j0 + 3];

        ++i;
        j0 += 4;
    }
    return *this;
}
