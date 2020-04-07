#ifndef BAND_MATR_H
#define BAND_MATR_H

#include <assert.h>

// We consider a special case of banded matris
// that initially has only d0+d1 nonzero diagonals:
//     d0 diagonals below that main one, indices (i, i-d0..i-1);
//     the main diagonal, indices (i, i);
//     d1 diagonals above the main, including main, indices (i, i..i+d1-1).
// The Gauss eliminating with pivoting and reordering needs
// a space for d0 additional diagonal, total (i, i+d0+d1-1).
// So the total space needed
// (without the right column) equals n*(d0+d1).
class BandExtMatrix {
public:
    int n;              // Matrix size
    int d0;             // Number of digonals below the main diagonal,
                        //        not including the main diagonal
    int d1;             // Number of digonals above the main diagonal,
                        //        including the main diagonal
    int numDiags;       // d0 + d1
    double* elements;   //
    double* r;          // Free terms column
private:
    BandExtMatrix();

public:
    BandExtMatrix(int s, int diags0, int diags1):
        n(s),
        d0(diags0),
        d1(diags1),
        numDiags(2*d0 + d1),
        elements(new double[n*numDiags]),
        r(new double[n])
    {
        int k = n*numDiags;
        for (int i = 0; i < k; ++i)
            elements[i] = 0.;
        for (int i = 0; i < n; ++i)
            r[i] = 0.;
    }

    BandExtMatrix(const BandExtMatrix& m):
        n(m.n),
        d0(m.d0),
        d1(m.d1),
        numDiags(m.numDiags),
        elements(new double[n*numDiags]),
        r(new double[n])
    {
        int k = n*numDiags;
        for (int i = 0; i < k; ++i)
            elements[i] = m.elements[i];
        for (int i = 0; i < n; ++i)
            r[i] = m.r[i];
    }

    BandExtMatrix& operator=(const BandExtMatrix& m) {
        assert(n == m.n && d0 == m.d0 && d1 == m.d1);
        int k = n*numDiags;
        for (int i = 0; i < k; ++i)
            elements[i] = m.elements[i];
        for (int i = 0; i < n; ++i)
            r[i] = m.r[i];
        return *this;
    }

    ~BandExtMatrix() {
        delete[] elements;
        delete[] r;
    }

    void init() {
        int k = n*numDiags;
        for (int i = 0; i < k; ++i)
            elements[i] = 0.;
        for (int i = 0; i < n; ++i)
            r[i] = 0.;
    }

    int numElems() const {
        return (n*numDiags);
    }

    void resize(int s, int diags0, int diags1) {
        if (s == n && diags0 == d0 && diags1 == d1) {
            init();
            return;
        }
        int nDiags = 2*diags0 + diags1;
        int k = s*nDiags;
        if (k > numElems()) {
            delete[] elements;
            elements = new double[k];
        }
        if (s > n) {
            delete[] r;
            r = new double[s];
        }
        n = s;
        d0 = diags0;
        d1 = diags1;
        numDiags = nDiags;
        init();
    }

    double& at(int i, int j) {
        assert(0 <= i && i < n);
        assert(0 <= j && j < n);
        assert(i-d0 <= j && j <= i+d0+d1-1);

        return elements[i*numDiags + (j-i) + d0];
    }

    double at(int i, int j) const {
        assert(0 <= i && i < n);
        assert(0 <= j && j < n);
        assert(i-d0 <= j && j <= i+d0+d1-1);

        return elements[i*numDiags + (j-i) + d0];
    }

    double& right(int i) {
        assert(0 <= i && i < n);
        return r[i];
    }

    const double& right(int i) const {
        assert(0 <= i && i < n);
        return r[i];
    }

    bool gauss(); // Returns (det() != 0)

    bool solve(
        double* p
    );

    void multiply(const double* p, double* q) const;

    void print() const;
};

#endif
