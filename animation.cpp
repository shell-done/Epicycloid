#include "animation.h"

Animation::Animation(Function* x, Function* y, QWidget *parent) : QWidget(parent), xt(x), yt(y) {
    this->setMinimumSize(1800, 940);
    circles = new QPixmap(1800, 940);
    drawing = new QPixmap(1800, 940);
    drawing->fill(Qt::white);

    circlesNumber = 50;

    timer = new QTimer(this);
    timer->start(15);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    t = 0;

    solidRedPen.setColor(Qt::red);
    solidRedPen.setWidth(3);
    solidGreenPen.setColor(Qt::green);
    solidGreenPen.setWidth(3);
    solidBlackPen.setColor(Qt::black);
    solidBlackPen.setWidth(2);
    dashBlackPen.setColor(Qt::black);
    dashBlackPen.setStyle(Qt::DashLine);
    dashBlackPen.setWidth(1);
}

Animation::~Animation() {
    delete circles;
    delete drawing;
}

void Animation::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    circles->fill(Qt::transparent);
    QPainter painter(circles);

    QPointF x(900, xt->getv0());
    QPointF y(yt->getv0(), 700);
    double omega0 = 2*M_PI/xt->getPeriod();

    for(int n=1; n<xt->getn() && n<=circlesNumber; n++) {
        x = drawCircle(&painter, x, xt->getCn(n), n*omega0*t + xt->getTetan(n));
        y = drawCircle(&painter, y, yt->getCn(n), n*omega0*t + yt->getTetan(n));
    }
    painter.setPen(solidRedPen);
    painter.setBrush(QBrush());
    painter.drawEllipse(x, 3, 3);
    painter.drawEllipse(y, 3, 3);

    painter.setPen(solidBlackPen);
    painter.drawLine(QPointF(x.x(), 0), QPointF(x.x(), circles->height()));
    painter.drawLine(QPointF(0, y.y()), QPointF(circles->width(), y.y()));

    QPainter drawingPainter(drawing);
    drawingPainter.setPen(solidBlackPen);
    drawingPainter.setBrush(QBrush(Qt::black));
    drawingPainter.drawEllipse(QPointF(x.x(), y.y()), 2, 2);

    QPainter widgetPainter(this);
    widgetPainter.drawPixmap(0, 0, *drawing);
    widgetPainter.drawPixmap(0, 0, *circles);

    t+=2;
}

QPointF Animation::drawCircle(QPainter* painter, QPointF p, double radius, double angle) {
    painter->setPen(solidGreenPen);
    painter->setBrush(QBrush(Qt::green));
    painter->drawEllipse(p, 3, 3);

    painter->setPen(solidBlackPen);
    painter->setBrush(QBrush());
    painter->drawEllipse(p, radius, radius);

    painter->setPen(dashBlackPen);
    QPointF nextP(p.x() + radius*qCos(angle), p.y() + radius*qSin(angle));
    painter->drawLine(p, nextP);

    return nextP;
}

void Animation::setCirclesNumber(int n) {drawing->fill(Qt::white); circlesNumber = n;}
