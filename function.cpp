#include "function.h"

Function::Function(QList<QPoint>& p, int n, double delta) : points(p) {
    Q_ASSERT_X(p.size(), "Fourier initialization", "Points list is empty");
    Q_ASSERT_X(points.first().y() == points.last().y(), "Fourier initialization", "The first and last points do not have the same y");

    period = points.last().x();
    getFourierCoeff(n, delta);
}

void Function::getFourierCoeff(int precision, double delta) {
    //Calculation of v0
    v0 = 0;
    for(int i=0; i<points.size() - 1; i++)
        v0 += (points[i].y() + points[i+1].y())*static_cast<double>((points[i+1].x() - points[i].x())/2.0);
    v0 /= period;

    //Calculation of An and Bn coeffs
    double omega0 = 2.0*M_PI/period;
    An.prepend(0);
    Bn.prepend(0);
    for(int n=1; n<=precision; n++) {
        double an = 0;
        double bn = 0;

        for(int r=0; r<points.size() - 1; r++) {
            double u = (points[r+1].y() - points[r].y())/static_cast<double>(1.0*points[r+1].x() - points[r].x());
            double v = points[r].y() - points[r].x()*u;

            for(double t=points[r].x(); t<points[r+1].x(); t+=delta) {
                double cosnwt = qCos(n*omega0*t);
                double sinnwt = qSin(n*omega0*t);

                an += (u*t + v)*cosnwt*delta;
                bn += (u*t + v)*sinnwt*delta;
            }
        }

        an += points.last().y()*qCos(n*omega0*period)*delta;
        bn += points.last().y()*qSin(n*omega0*period)*delta;

        An += an*2.0/period;
        Bn += bn*2.0/period;
    }
}

int Function::getn() {Q_ASSERT_X(An.size() == Bn.size(), "getn", "An and Bn list do not have the same size"); return An.size();}
int Function::getPeriod() {return period;}
double Function::getv0() {return v0;}
double Function::getAn(int n) {return An[n];}
double Function::getBn(int n) {return Bn[n];}

void Function::toCosSum() {
    Cn.clear();
    tetan.clear();
    Cn.prepend(0);
    tetan.prepend(0);

    for(int n=1; n<An.size(); n++) {
        Cn += qSqrt(qPow(An[n], 2) + qPow(Bn[n], 2));
        tetan += -qAtan(Bn[n]/An[n]);
    }
}

void Function::toSinSum() {
    Cn.clear();
    tetan.clear();
    Cn.prepend(0);
    tetan.prepend(0);

    for(int n=1; n<An.size(); n++) {
        Cn += qSqrt(qPow(An[n], 2) + qPow(Bn[n], 2));
        tetan += qAtan(An[n]/Bn[n]);
    }
}

void Function::addPi(int n) {tetan[n] += M_PI;}

double Function::getCn(int n) {return Cn[n];}
double Function::getTetan(int n) {return tetan[n];}
