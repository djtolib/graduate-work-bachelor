#ifndef CUBIC_POL_H
#define CUBIC_POL_H

#include <vector>
#include "R2Graph.h"
#include "bandmatrix.h"

class CubicPolynomial {
public:
    double coeff[4];

public:
    CubicPolynomial() {
        for (int i = 0; i < 4; ++i)
            coeff[i] = 0.;
    }

    CubicPolynomial(const double* c) {
        for (int i = 0; i < 4; ++i)
            coeff[i] = c[i];
    }

    CubicPolynomial(const std::vector<double>& c) {
        for (int i = 0; i < 4; ++i)
            coeff[i] = c[i];
    }

    double value(double t) const {
        double v = 0.;
        for (int i = 3; i >= 0; --i) {
            v *= t;
            v += coeff[i];
        }
        return v;
    }

    double operator[](int i) const { return coeff[i]; }
    double& operator[](int i) { return coeff[i]; }

    double operator()(double t) const { return value(t); }

    // Interpolate so that
    //     p(a) = value0, dp(a) = derivative0
    //     p(b) = value0, dp(b) = derivative1
    //
    CubicPolynomial& interpolate(
        double a, double b,
        double value0, double derivative0,
        double value1, double derivative1
    );

    // Derivative of polynomial:
    CubicPolynomial derivative() const {
        double c[4];
        c[0] = coeff[1];
        c[1] = 2.*coeff[2];
        c[2] = 3.*coeff[3];
        c[3] = 0.;
        return CubicPolynomial(c);
    }

    // Second Derivative
    CubicPolynomial derivative2() const {
        double c[4];
        c[0] = 2*coeff[2];
        c[1] = 2.*3.*coeff[3];
        c[2] = 0.;
        c[3] = 0.;
        return CubicPolynomial(c);
    }

    // Derivative value
    double derivativeValue(double x) const {
        return ((3.*coeff[3]*x + 2.*coeff[2])*x + coeff[1]);
    }

    // Value of second derivative
    double derivative2Value(double x) const {
        return (6.*coeff[3]*x + 2*coeff[2]);
    }
};

class CubicSpline {
public:
    int numNodes;
    R2Point* nodes;                 // array of numNodes size
    CubicPolynomial* polynomials;   // array of numNodes-1 size

private:
    R2Vector* directions;
    BandExtMatrix* bandMatrix;  // C2 Spline: to solve a linear system
    double* coeffs;             // C2 Spline: to solve a linear system

public:
    CubicSpline():
        numNodes(0),
        nodes(0),
        polynomials(0),
        directions(0)
    {}

    CubicSpline(int n):
        numNodes(n),
        nodes(new R2Point[n]),
        polynomials(new CubicPolynomial[n]),
        directions(new R2Vector[n]),
        bandMatrix(0),
        coeffs(0)
    {}

    ~CubicSpline() {
        delete[] nodes;
        delete[] polynomials;
        delete[] directions;
        delete bandMatrix;
        delete[] coeffs;
    }

    void resize(int n);

    // Value of the spline
    // If nodeIdx >= 0, then x belongs 
    // to the interval nodes[nodeIdx].x <= x <= nodes[nodeIdx+1].x
    double value(double x, int nodeIdx = (-1)) const;

    // Calculate cubic polynomials so the the spline will be C1-continues
    CubicSpline& interpolateC1();

    // Calculate cubic polynomials so the the spline will be C2-continues,
    // solving a system of linear equarions with band 9-diagonal matrix
    CubicSpline& interpolateC2();

private:
    int findSegment(double x) const;    // Binary search
};

#endif
