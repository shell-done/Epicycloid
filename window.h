#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include "eimage.h"
#include "animation.h"

class Window : public QMainWindow {
    Q_OBJECT

public:
    Window(QWidget* parent = nullptr);
    ~Window();

private:
    //------------------Methods-----------------
    //Initialize the window
    void initWidgets();
    void processingFinished();

    //------------------------------------------
    //------------Member variables--------------
    //Window widgets
    QTabWidget* tabs;

    QWidget* imageWidget;
    QGridLayout* imageAreaLayout;
    QHBoxLayout* stepLayout;
    EImage* image;

    QWidget* animationWidget;
    QGridLayout* animationLayout;
    QHBoxLayout* sliderLayout;
    QSlider* slider;
    QLabel* sliderValue;
    Animation* animation;

    QPushButton* nextStep;
    QProgressBar* processed;
    unsigned int step;
    QStringList stepsTitle;

    QLabel* processStep;
    QPushButton* prevProcessed;
    QPushButton* nextProcessed;

    //File used
    QFileInfo imageFile;

private slots:
    void onButtonClicked();
    void getImagePath();

    void nextImg();
    void prevImg();

    void setCirclesNumber(int);
};

#endif // WINDOW_H
