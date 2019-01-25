#ifndef ANIMATION_H
#define ANIMATION_H

#include <QtWidgets>
#include "function.h"

class Animation : public QWidget {
    Q_OBJECT

public:
    Animation(Function* xt, Function* yt, QWidget *parent = nullptr);
    ~Animation();
    void setCirclesNumber(int n);

protected:
    void paintEvent(QPaintEvent* event);

private:
    QPointF drawCircle(QPainter* painter, QPointF p, double radius, double angle);

    QPixmap* circles;
    QPixmap* drawing;

    QTimer* timer;
    double t;
    int circlesNumber;

    QPen solidRedPen;
    QPen solidGreenPen;
    QPen solidBlackPen;
    QPen dashBlackPen;

    Function* xt;
    Function* yt;
};

#endif // ANIMATION_H
