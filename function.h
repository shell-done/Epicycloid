#ifndef FOURIER_H
#define FOURIER_H

#include <QList>
#include <QPoint>
#include <QtMath>
#include <QtDebug>

class Function {
public:
    Function(QList<QPoint>& p, int n, double delta);
    int getn();
    int getPeriod();
    double getv0();

    double getAn(int n);
    double getBn(int n);

    void toCosSum();
    void toSinSum();
    void addPi(int n);
    double getCn(int n);
    double getTetan(int n);

protected:
    void getFourierCoeff(int precision, double delta);

    QList<QPoint> points;
    int period;

    QList<double> An;
    QList<double> Bn;
    double v0;

    QList<double> Cn;
    QList<double> tetan;
};

#endif // FOURIER_H
