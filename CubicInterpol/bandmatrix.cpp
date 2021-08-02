#include <stdio.h>
#include <math.h>
#include "bandmatrix.h"

static const double EPS = 1e-8;

bool BandExtMatrix::gauss() {
    int i = 0;
    while (i < n) {
        double maxelem = fabs(at(i, i));
        int maxidx = i;
        int im = i + d0;
        if (im > n - 1)
            im = n - 1;
        for (int k = i+1; k <= im; ++k) {
            if (fabs(at(k, i)) > maxelem) {
                maxelem = fabs(at(k, i));
                maxidx = k;
            }
        }
        if (maxelem <= EPS) {
            // Zero column: Should NOT come here for non-degenerated matrix.
            return false;       // Zero determinant
        }
        int jm = i + d0 + d1 - 1;
        if (jm > n - 1)
            jm = n - 1;
        if (maxidx != i) {
            //... swapRows(i, maxidx);
            for (int j = i; j <= jm; ++j) {
                double tmp = at(i, j);
                at(i, j) = at(maxidx, j);
                at(maxidx, j) = (-tmp);
            }
            double tmp = right(i);
            right(i) = right(maxidx);
            right(maxidx) = (-tmp);
        }
        double z = at(i, i);
        assert(fabs(z) > EPS);
        for (int k = i+1; k <= im; ++k) {
            double c = (-at(k, i) / z);
            at(k, i) = 0.;
            for (int j = i+1; j <= jm; ++j) {
                at(k, j) += at(i, j)*c;
            }
            r[k] += r[i] * c;
        }
        ++i;
    }
    return (fabs(at(n-1, n-1)) > EPS);  // Must be true!
}

bool BandExtMatrix::solve(
    double* p
) {
    // For test
    //... BandExtMatrix initialMatrix = *this;

    bool res = gauss();

    //... printf("gauss:\n");
    //... print();

    if (!res)
        return false;
    for (int i = n-1; i >= 0; --i) {
        int jm = i+d0+d1-1;
        if (jm > n-1)
            jm = n-1;
        double s = r[i];
        for (int j = i+1; j <= jm; ++j) {
            s -= p[j]*at(i, j);
        }
        p[i] = s * (1./at(i, i));
    }

    // Test
    /*...
    double* q = new double[n];
    initialMatrix.multiply(p, q);
    printf("Test of solve: product of matrix and variable column\n");
    for (int i = 0; i < n; ++i) {
        printf("%9.3f ", q[i]);
    }
    printf("\nMust be\n");
    for (int i = 0; i < n; ++i) {
        printf(
            "%9.3f ",
            initialMatrix.right(i)
        );
    }
    printf("---\n");
    delete[] q;
    ...*/

    return true;
}

void BandExtMatrix::multiply(const double* p, double* q) const {
    for (int i = 0; i < n; ++i) {
        q[i] = 0;
        int jLeft = i - d0;
        if (jLeft < 0)
            jLeft = 0;
        int jRight = i + d0 + d1 - 1;
        if (jRight > n - 1)
            jRight = n - 1;
        for (int j = jLeft; j <= jRight; ++j) {
            q[i] += at(i, j)*p[j];
        }
    }
}

void BandExtMatrix::print() const {
    //... printf("Matrix:\n");
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            double v = 0.;
            if (i - d0 <= j && j <= i + d0 + d1 - 1)
                v = at(i, j); 
            printf("%9.3f ", v);
        }
        printf(" | %9.3f\n", right(i));
    }
    printf("----\n");
    fflush(stdout);
}
