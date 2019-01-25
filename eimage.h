#ifndef EIMAGE_H
#define EIMAGE_H

#include <QtWidgets>
#include "function.h"

class EImage : public QWidget {
    Q_OBJECT

public:
    EImage(QString filename, QWidget* parent = nullptr);
    ~EImage();

    //------------------Methods-----------------
    //Image processing
    void toShadesOfGray();
    void thresholding(int threshold);
    void gradient();
    void reverse();
    void findPoints();
    void smoothCurve(float angle);
    void recreateShape();
    void drawGraph();
    void drawFourierGraph();
    void addPhases();

    Function* getXt();
    Function* getYt();

    void goToImg(int n);

protected:
    void paintEvent(QPaintEvent*);

private:
    //------------------Methods-----------------
    //Image processing
    QImage gradientX();
    QImage gradientY();

    void ordonatePoints();

    //------------Member variables--------------
    QList<QImage*> images;
    QImage* currentImage;

    QList<QPoint> edgePoints;
    QList<int> distances;

    int period;
    QList<QPoint> xPoints;
    QList<QPoint> yPoints;

    int precision;
    Function* xt;
    Function* yt;
};

#endif // EIMAGE_H
