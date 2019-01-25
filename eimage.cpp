#include "eimage.h"

EImage::EImage(QString filename, QWidget* parent): QWidget(parent) {
    QImage temp(filename);
    double newHeight = 900.0/temp.width()*temp.height();
    double newWidth = 480.0/temp.height()*temp.width();

    if(newHeight*900 < newWidth*480)
        currentImage = new QImage(temp.scaled(900, static_cast<int>(newHeight)));
    else
        currentImage = new QImage(temp.scaled(static_cast<int>(newWidth), 480));

    Q_ASSERT_X(!currentImage->isNull(), "EImage initialization", "currentImage is null");

    setMinimumSize(currentImage->size());
    images.prepend(currentImage);
}

EImage::~EImage() {
    for(QImage* img: images)
        delete img;

    delete xt;
    delete yt;
}

void EImage::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.drawPixmap(0, 0, QPixmap::fromImage(*currentImage));
}

void EImage::toShadesOfGray() {
    QImage* newImage = new QImage(currentImage->convertToFormat(QImage::Format_Grayscale8));

    images += newImage;
    currentImage = newImage;

    update();
}

void EImage::thresholding(int threshold) {
    QImage* newImage = new QImage(*currentImage);

    for(int i=0; i<newImage->width(); i++)
        for(int j=0; j<newImage->height(); j++)
            if(newImage->pixelColor(i, j).red() < threshold)
                newImage->setPixelColor(i, j, QColor(0, 0, 0));
            else
                newImage->setPixelColor(i, j, QColor(255, 255, 255));

    images += newImage;
    currentImage = newImage;

    update();
}

QImage EImage::gradientX() {
    QImage image(currentImage->size(), currentImage->format());

    for(int i=0; i<image.width(); i++)
        for(int j=0; j<image.height(); j++)
            if(i == 0 || i == image.width() - 1)
                image.setPixelColor(i, j, currentImage->pixelColor(i, j));
            else {
                int val = qAbs(currentImage->pixelColor(i + 1, j).red() - currentImage->pixelColor(i - 1, j).red());
                image.setPixelColor(i, j, QColor(val, val, val));
            }

    return image;
}

QImage EImage::gradientY() {
    QImage image(currentImage->size(), currentImage->format());

    for(int i=0; i<image.width(); i++)
        for(int j=0; j<image.height(); j++)
            if(j == 0 || j == image.height() - 1)
                image.setPixelColor(i, j, currentImage->pixelColor(i, j));
            else {
                int val = qAbs(currentImage->pixelColor(i, j + 1).red() - currentImage->pixelColor(i, j - 1).red());
                image.setPixelColor(i, j, QColor(val, val, val));
            }
    return image;
}

void EImage::gradient() {
    QImage* newImage = new QImage(currentImage->convertToFormat(QImage::Format_Grayscale8));

    QImage gradientXImage = gradientX();
    QImage gradientYImage = gradientY();

    for(int i=0; i<newImage->width(); i++)
        for(int j=0; j<newImage->height(); j++) {
            int val = static_cast<int>(qSqrt(qPow(gradientXImage.pixelColor(i, j).red(), 2) + qPow(gradientYImage.pixelColor(i, j).red(), 2)));
            val = qMin(255, val);

            newImage->setPixelColor(i, j, QColor(val, val, val));
        }

    images += newImage;
    currentImage = newImage;

    update();
}

void EImage::reverse() {
    QImage* newImage = new QImage(currentImage->convertToFormat(QImage::Format_Grayscale8));

    for(int i=1; i<newImage->width() - 1; i++)
        for(int j=1; j<newImage->height() - 1; j++) {
            int val = 255 - newImage->pixelColor(i, j).red();
            newImage->setPixelColor(i, j, QColor(val, val, val));
        }

    images += newImage;
    currentImage = newImage;

    update();
}

void EImage::findPoints() {
    QImage* newImage = new QImage(currentImage->convertToFormat(QImage::Format_RGB32));
    currentImage = newImage;
    images += currentImage;

    QList<QPoint> edgePointsChild;

    for(int i=100; i<currentImage->width(); i++)
        for(int j=0; j<currentImage->height(); j++)
            if(currentImage->pixelColor(i, j) == Qt::black) {
                edgePoints += QPoint(i, j);
                edgePointsChild += QPoint(-1, -1);
                i = currentImage->width();
                break;
            }

    int currentIdx = 0;
    while(currentIdx != -1) {
        QPoint currentPoint = edgePoints[currentIdx];
        currentImage->setPixelColor(currentPoint, Qt::blue);

        for(int i=-2; i<=2; i++)
            for(int j=-2; j<=2; j++) {
                int x = currentPoint.x() + i;
                int y = currentPoint.y() + j;

                if(x < 0 || y < 0 || x > currentImage->width() - 1 || y > currentImage->height() - 1)
                    continue;

                if(currentImage->pixelColor(x, y) != Qt::black)
                    continue;

                currentImage->setPixelColor(x, y, Qt::red);
            }

        int bestDistance = 100;
        QPoint closestPoint(-1, -1);
        for(int i=-5; i<=5; i++)
            for(int j=-5; j<=5; j++) {
                int x = currentPoint.x() + i;
                int y = currentPoint.y() + j;

                if(x < 0 || y < 0 || x > currentImage->width() - 1 || y > currentImage->height() - 1)
                    continue;

                if(currentImage->pixelColor(x, y) != Qt::black && currentImage->pixelColor(x, y) != Qt::green)
                    continue;

                if(i*i + j*j < bestDistance) {
                    closestPoint = QPoint(x, y);
                    bestDistance = i*i + j*j;
                }
            }

        if(bestDistance != 100) {
            if(currentIdx != 0)
                currentImage->setPixelColor(closestPoint, Qt::green);

            edgePointsChild[currentIdx] = closestPoint;
            edgePoints += closestPoint;
            edgePointsChild += QPoint(-1, -1);
            currentIdx++;
        }
        if(bestDistance == 100) {
            if(qPow(currentPoint.x() - edgePoints[0].x(), 2) + qPow(currentPoint.y() - edgePoints[0].y(), 2) <= 25) {
                qDebug() << "Path found";
                currentIdx = -1;
                break;
            }
            edgePoints.removeLast();
            edgePointsChild.removeLast();

            currentIdx--;

            if(currentIdx <= 1) {
                qDebug() << "No path found";
                currentIdx = -1;
            }
        }
    }
}

void EImage::smoothCurve(float maxAngle) {
    for(int i=1; i<edgePoints.size() + 2; i++) {
        int idxP = i%(edgePoints.size() - 1);
        int idxNextP = (i+1)%(edgePoints.size() - 1);
        int idxPrevP = (i-1)%(edgePoints.size() - 1);

        QPoint vectNextPP(edgePoints[idxNextP].x() - edgePoints[idxP].x(), edgePoints[idxNextP].y() - edgePoints[idxP].y());
        QPoint vectNextPPrevP(edgePoints[idxNextP].x() - edgePoints[idxPrevP].x(), edgePoints[idxNextP].y() - edgePoints[idxPrevP].y());
        qreal distNextPP = qSqrt(qPow(edgePoints[idxNextP].x() - edgePoints[idxP].x(), 2) + qPow(edgePoints[idxNextP].y() - edgePoints[idxP].y(), 2));
        qreal distNextPPrevP = qSqrt(qPow(edgePoints[idxNextP].x() - edgePoints[idxPrevP].x(), 2) + qPow(edgePoints[idxNextP].y() - edgePoints[idxPrevP].y(), 2));

        qreal dotProduct = vectNextPP.x()*vectNextPPrevP.x() + vectNextPP.y()*vectNextPPrevP.y();
        qreal cosangle = dotProduct/(distNextPP*distNextPPrevP);
        float angle = static_cast<float>(qAcos(cosangle));

        if(angle < maxAngle) {
            i--;
            edgePoints.removeAt(idxP);
        }
    }

    QImage* newImage = new QImage(currentImage->size(), QImage::Format_RGB32);
    currentImage = newImage;
    images += currentImage;

    newImage->fill(Qt::white);
    for(QPoint p: edgePoints)
        newImage->setPixelColor(p, Qt::black);
}

void EImage::recreateShape() {
    QPixmap* remadeShape = new QPixmap(currentImage->width(), currentImage->height());
    QPainter painter(remadeShape);
    QPen pen(Qt::red);
    pen.setWidth(3);
    painter.setPen(pen);
    painter.setBrush(QBrush(Qt::red));

    for(int i=0; i<edgePoints.size(); i++) {
        int nextIdx = (i+1)%edgePoints.size();
        painter.drawEllipse(edgePoints[i], 5, 5);
        painter.drawLine(edgePoints[i], edgePoints[nextIdx]);
    }

    pen.setColor(Qt::blue);
    painter.setBrush(QBrush(Qt::blue));
    painter.setPen(pen);
    painter.drawEllipse(edgePoints[0], 5, 5);

    pen.setColor(Qt::green);
    painter.setBrush(QBrush(Qt::green));
    painter.setPen(pen);
    painter.drawEllipse(edgePoints[edgePoints.size() - 1], 5, 5);

    QImage* newImage = new QImage(remadeShape->toImage());
    currentImage = newImage;
    images += currentImage;

    painter.end();
    delete remadeShape;
}

void EImage::drawGraph() {
    QPixmap* graph = new QPixmap(currentImage->size());

    period = 0;
    for(int i=0; i<edgePoints.size(); i++) {
        int nextIdx = (i+1)%edgePoints.size();

        qreal dist = qSqrt(qPow(edgePoints[i].x() - edgePoints[nextIdx].x(), 2) + qPow(edgePoints[i].y() - edgePoints[nextIdx].y(), 2));
        distances.append(static_cast<int>(dist));
        period += static_cast<int>(dist);
    }

    float xCoeff = static_cast<float>((currentImage->width() - 80)*1.0/period);

    QPen pen(Qt::black);
    pen.setWidth(2);
    QPainter painter(graph);

    int currentX = 0;
    for(int i=0; i<edgePoints.size(); i++) {
        xPoints += QPoint(currentX, edgePoints[i].x());
        yPoints += QPoint(currentX, edgePoints[i].y());

        currentX += distances[i];
    }
    xPoints += QPoint(currentX, edgePoints[0].x());
    yPoints += QPoint(currentX, edgePoints[0].y());

    Q_ASSERT_X(xPoints.size() == yPoints.size(), "Graph initialization", "The number of points in xPoints and yPoints is different");

    for(int i=0; i<xPoints.size() - 1; i++) {
        painter.drawEllipse(xPoints[i].y(), yPoints[i].y(), 5, 5);
        painter.drawLine(xPoints[i].y(), yPoints[i].y(), xPoints[i+1].y(), yPoints[i+1].y());
    }

    for(int i=0; i<edgePoints.size() - 1; i++) {
        pen.setColor(Qt::red);
        painter.setPen(pen);
        painter.setBrush(Qt::red);
        painter.drawEllipse(QPoint(40 + static_cast<int>(xPoints[i].x()*xCoeff), graph->height() - xPoints[i].y()), 5, 5);
        painter.drawLine(40 + static_cast<int>(xPoints[i].x()*xCoeff), graph->height() - xPoints[i].y(),
                         40 + static_cast<int>(xPoints[i + 1].x()*xCoeff), graph->height() - xPoints[i + 1].y());

        pen.setColor(Qt::blue);
        painter.setPen(pen);
        painter.setBrush(Qt::blue);
        painter.drawEllipse(QPoint(40 + static_cast<int>(yPoints[i].x()*xCoeff), yPoints[i].y()), 5, 5);
        painter.drawLine(40 + static_cast<int>(yPoints[i].x()*xCoeff), yPoints[i].y(),
                         40 + static_cast<int>(yPoints[i + 1].x()*xCoeff), yPoints[i + 1].y());
    }

    QImage* newImage = new QImage(graph->toImage());
    painter.end();
    delete graph;

    currentImage = newImage;
    images += currentImage;
}

void EImage::drawFourierGraph() {
    QPixmap* fourierPixmap = new QPixmap(currentImage->size());
    fourierPixmap->fill(Qt::gray);

    xt = new Function(xPoints, 50, 0.01);
    yt = new Function(yPoints, 50, 0.01);

    QPainter painter(fourierPixmap);
    QPen pen(Qt::black);
    pen.setWidth(2);
    painter.setPen(pen);
    painter.drawLine(40, 20, 40, fourierPixmap->height() - 20);
    painter.drawLine(20, fourierPixmap->height() - 40, fourierPixmap->width() - 40, fourierPixmap->height() - 40);

    double xCoeff = (currentImage->width() - 80)*1.0/period;
    double omega0 = 2*M_PI/period;

    for(double t=0; t<period; t++) {
        double x = xt->getv0();
        double y = yt->getv0();

        for(int n=1; n<xt->getn(); n++) {
            double cosnwt = qCos(n*omega0*t);
            double sinnwt = qSin(n*omega0*t);

            x += xt->getAn(n)*cosnwt + xt->getBn(n)*sinnwt;
            y += yt->getAn(n)*cosnwt + yt->getBn(n)*sinnwt;
        }

        painter.setPen(Qt::red);
        painter.setBrush(Qt::red);
        painter.drawEllipse(40 + static_cast<int>(t*xCoeff), static_cast<int>(height() - x), 2, 2);

        painter.setPen(Qt::blue);
        painter.setBrush(Qt::blue);
        painter.drawEllipse(40 + static_cast<int>(t*xCoeff), static_cast<int>(y), 2, 2);
    }

    QImage* newImage = new QImage(fourierPixmap->toImage());
    painter.end();
    delete fourierPixmap;

    currentImage = newImage;
    images += newImage;
}

void EImage::addPhases() {
    xt->toCosSum();
    yt->toSinSum();

    QPixmap* signalPixmap = new QPixmap(currentImage->size());
    signalPixmap->fill(Qt::gray);

    QPainter painter(signalPixmap);
    QPen pen(Qt::black);
    pen.setWidth(2);
    painter.setPen(pen);
    painter.drawLine(40, 20, 40, signalPixmap->height() - 20);
    painter.drawLine(20, signalPixmap->height() - 40, signalPixmap->width() - 40, signalPixmap->height() - 40);

    double xCoeff = (currentImage->width() - 80)*1.0/period;
    double omega0 = 2*M_PI/period;

    for(double t=0; t<period; t++) {
        for(int n=1; n<xt->getn(); n++) {
            double cosnwt = qCos(n*omega0*t);
            double sinnwt = qSin(n*omega0*t);

            if((xt->getAn(n)*cosnwt + xt->getBn(n)*sinnwt)/(xt->getCn(n)*qCos(n*omega0*t + xt->getTetan(n))) < 0)
                xt->addPi(n);
            if((yt->getAn(n)*cosnwt + yt->getBn(n)*sinnwt)/(yt->getCn(n)*qSin(n*omega0*t + yt->getTetan(n))) < 0)
                yt->addPi(n);
        }
    }

    for(int t=0; t<period; t++) {
        double x = xt->getv0();
        double y = yt->getv0();

        for(int n=1; n<xt->getn(); n++) {
            x += xt->getCn(n)*qCos(n*omega0*t + xt->getTetan(n));
            y += yt->getCn(n)*qSin(n*omega0*t + yt->getTetan(n));
        }

        painter.setPen(Qt::red);
        painter.setBrush(Qt::red);
        painter.drawEllipse(40 + static_cast<int>(t*xCoeff), static_cast<int>(height() - x), 2, 2);

        painter.setPen(Qt::blue);
        painter.setBrush(Qt::blue);
        painter.drawEllipse(40 + static_cast<int>(t*xCoeff), static_cast<int>(y), 2, 2);
    }

    QImage* newImage = new QImage(signalPixmap->toImage());
    painter.end();
    delete signalPixmap;

    currentImage = newImage;
    images += newImage;
}

Function* EImage::getXt() {return xt;}
Function* EImage::getYt() {return yt;}

void EImage::goToImg(int n) {
    currentImage = images[n];
}
