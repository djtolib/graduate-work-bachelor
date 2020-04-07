#ifndef REAL_PIXEL
#define REAL_PIXEL

#include <cmath>

inline double sigmoid(double x, double coeff = 1.0) {
    return 1./(1. + exp(-(x-0.5)*coeff));
}

class RealPixel {
public:
    double redValue;    // цветовые компоненты
    double greenValue;  // в диапазоне [0.0, 1.0]
    double blueValue;
    // конструктор класса без параметров
    RealPixel():
        redValue(0.),
        greenValue(0.),
        blueValue(0.)
    {}

    // конструктор с параметрами
    RealPixel(double r, double g, double b):
        redValue(r),
        greenValue(g),
        blueValue(b)
    {}

    // Конструктор с целочисленными
    // параметрам, которые переводятся
    // в вещественное число
    RealPixel(int r, int g, int b):
        redValue(r/255.),
        greenValue(g/255.),
        blueValue(b/255.)
    {}

    // Следующая группа методов
    // назначают соответсвующие
    // компоненты цвета
    void setRed(double r) { redValue = r; }
    void setGreen(double g) { greenValue = g; }
    void setBlue(double b) { blueValue = b; }

    // Этот метод назначает сразу
    // все компоненты цвета
    void setRGB(double r, double g, double b) { 
        redValue = r;
        greenValue = g;
        blueValue = b; 
    }

    // Доступ к членам класса
    double red() const { return redValue; }
    double green() const { return greenValue; }
    double blue() const { return blueValue; }

    // Переопределение операции *
    RealPixel operator*(double a) const {
        return RealPixel(
            red()*a,
            green()*a,
            blue()*a
        );
    }

    // Переопределение операции *=
    RealPixel& operator*=(double a) {
        redValue *= a;
        greenValue *= a;
        blueValue *= a;
        return *this;
    }

    // Переопределение операции +
    RealPixel operator+(const RealPixel& p) const {
        return RealPixel(
            red() + p.red(),
            green() + p.green(),
            blue() + p.blue()
        );
    }

    // Переопределение операции +=
    RealPixel& operator+=(const RealPixel& p) {
        redValue += p.red();
        greenValue += p.green();
        blueValue += p.blue();
        return *this;
    }

    RealPixel& sigma(double coeff = 1.) {
        redValue = sigmoid(redValue, coeff);
        greenValue = sigmoid(greenValue, coeff);
        blueValue = sigmoid(blueValue, coeff);
        return *this;
    }

    // Назначение целочисленной компоненты цвета
    void setRed255(double r) { redValue = ((double) r)/255.; }
    void setGreen255(double g) { greenValue = ((double) g)/255.; }
    void setBlue255(double b) { blueValue = ((double) b)/255.; }

    // Назначение всех компонентов цвета
    void setRGB255(double r, double g, double b) {
        redValue = ((double) r)/255.;
        greenValue = ((double) g)/255.;
        blueValue = ((double) b)/255.;
    }

    // Следующие методы возвращают
    // соответствующие целочисленные компоненты
    int red255() const { 
        int n = (int)(redValue*255. + 0.5);
        if (n > 255)
            n = 255;
        else if (n < 0)
            n = 0;
        return n;
    }

    int green255() const { 
        int n = (int)(greenValue*255. + 0.5);
        if (n > 255)
            n = 255;
        else if (n < 0)
            n = 0;
        return n;
    }

    int blue255() const {
        int n = (int)(blueValue*255. + 0.5);
        if (n > 255)
            n = 255;
        else if (n < 0)
            n = 0;
        return n;
    }
};

void splineInterpolation(
    int imageWidth, int imageHeight,
    const RealPixel* imageMatrix,
    double zoom,
    double& realZoomX, double& realZoomY,
    int& zoomedWidth, int& zoomedHeight,
    RealPixel** zoomedMatrix,
    int splineType = 0  // 0 -- C2-cubic spline, 1 -- C1-spline
);

#endif
