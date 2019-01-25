#include "window.h"

Window::Window(QWidget *parent) : QMainWindow(parent) {
    step = 0;
    stepsTitle << "To gray scale" << "Threshold" << "Gradient" << "Threshold" << "Reverse colors" << "Find edges" << "Smooth curve"
               << "Recreate shape" << "Get graph" << "Fourier tranform" << "Add phases" << "";

    image = nullptr;
    animation = nullptr;
    processStep = nullptr;
    prevProcessed = nullptr;
    nextProcessed = nullptr;
    processed = nullptr;
    nextStep = nullptr;
    slider = nullptr;
    sliderValue = nullptr;
    sliderLayout = nullptr;
    animationLayout = nullptr;
    animationWidget = nullptr;
    stepLayout = nullptr;
    imageAreaLayout = nullptr;
    imageWidget = nullptr;
    tabs = nullptr;

    this->setWindowIcon(QIcon("icon.png"));
    this->setWindowTitle("Spine");
    initWidgets();
}

Window::~Window() {
    delete image;
    delete animation;
    delete processStep;
    delete prevProcessed;
    delete nextProcessed;
    delete processed;
    delete nextStep;
    delete slider;
    delete sliderValue;
    delete sliderLayout;
    delete animationLayout;
    delete animationWidget;
    delete stepLayout;
    delete imageAreaLayout;
    delete imageWidget;
    delete tabs;
}

void Window::initWidgets() {
    tabs = new QTabWidget(this);
    this->setCentralWidget(tabs);

    imageWidget = new QWidget(tabs);
    tabs->addTab(imageWidget, "Image");

    imageAreaLayout = new QGridLayout();
    imageWidget->setLayout(imageAreaLayout);

    this->showMaximized();

    processStep = new QLabel("NO_TEXT");
    processStep->setAlignment(Qt::AlignCenter);
    prevProcessed = new QPushButton(tr("Previous processed image"));
    nextProcessed = new QPushButton(tr("Next processed image"));

    animationWidget = new QWidget(tabs);
    animationLayout = new QGridLayout();
    animationWidget->setLayout(animationLayout);
    slider = new QSlider(Qt::Horizontal, animationWidget);
    slider->setRange(1, 50);
    slider->setMaximumWidth(700);
    slider->setMinimumWidth(400);
    slider->setValue(50);
    sliderValue = new QLabel(tr("Number of circles : 50"), animationWidget);
    connect(slider, SIGNAL(valueChanged(int)), this, SLOT(setCirclesNumber(int)));

    connect(prevProcessed, SIGNAL(clicked()), this, SLOT(prevImg()));
    connect(nextProcessed, SIGNAL(clicked()), this, SLOT(nextImg()));

    QTimer::singleShot(100, this, SLOT(getImagePath()));
}

void Window::getImagePath() {
    QMessageBox::information(this, "Spine - Information", tr("An image is needed, you need to give a picture with only one closed shape"));

    bool success = false;
    QString str = "C:/Users/alex1/Pictures/elephant.png";

    while(!success) {
        str = QFileDialog::getOpenFileName(this, tr("Spine - Open an image"), "", tr("Image Files (*.png *.jpg *.jpeg *.bmp *.gif *.pbm *.pgm *.ppm *.xbm *.xpm)"));
        if(str.isEmpty()) {
            qApp->quit();
            return;
        }

        QImage img(str);
        if(img.isNull()) {
            QMessageBox::warning(this, tr("Warning"), tr("This image doesn't exist or is unreadable."));
            this->close();
        }
        else
            success = true;
    }

    image = new EImage(str);
    imageFile.setFile(str);

    imageAreaLayout->addWidget(image, 0, 0, Qt::AlignCenter);
    this->setFocus();

    processed = new QProgressBar(this);
    processed->setMaximumWidth(width());
    processed->setRange(0, 100);
    processed->setMaximumWidth(1000);
    processed->setValue(0);
    imageAreaLayout->addWidget(processed, 1, 0);

    nextStep = new QPushButton(stepsTitle[static_cast<int>(step)], this);
    nextStep->setMaximumWidth(1000);
    nextStep->setFocus();
    connect(nextStep, SIGNAL(clicked()), this, SLOT(onButtonClicked()));
    imageAreaLayout->addWidget(nextStep, 2, 0);
}

void Window::onButtonClicked() {
    nextStep->setDisabled(true);

    switch(step) {
    case 0:
        image->toShadesOfGray();
        break;

    case 1:
        image->thresholding(200);
        break;

    case 2:
        image->gradient();
        break;

    case 3:
        image->thresholding(200);
        break;

    case 4:
        image->reverse();
        break;

    case 5:
        image->findPoints();
        break;

    case 6:
        image->smoothCurve(static_cast<float>(M_PI/25));
        break;

    case 7:
        image->recreateShape();
        break;

    case 8:
        image->drawGraph();
        break;

    case 9:
        image->drawFourierGraph();
        break;

    case 10:
        image->addPhases();
        break;

    case 11:
        sliderLayout = new QHBoxLayout();
        animation = new Animation(image->getXt(), image->getYt(), this);
        animationLayout->addWidget(animation, 0, 0, Qt::AlignCenter);
        animationLayout->addLayout(sliderLayout, 1, 0, Qt::AlignCenter);
        sliderLayout->addWidget(slider, 1, Qt::AlignRight);
        sliderLayout->addWidget(sliderValue, 1, Qt::AlignLeft);
        tabs->addTab(animationWidget, "Animation");
        tabs->setCurrentIndex(1);
        processed->setValue(100);
        nextStep->setDisabled(true);
        processingFinished();
        return;

    default:
        qDebug() << "[WARNING] No next step";
    }

    step++;
    nextStep->setText(stepsTitle[static_cast<int>(step)]);
    processed->setValue(processed->value() + 9);
    onButtonClicked();
}

void Window::processingFinished() {
    imageAreaLayout->removeWidget(nextStep);
    delete nextStep;
    nextStep = nullptr;

    stepLayout = new QHBoxLayout();
    stepLayout->addWidget(prevProcessed);
    stepLayout->addWidget(processStep);
    stepLayout->addWidget(nextProcessed);

    processStep->setText(stepsTitle[static_cast<int>(step - 1)]);
    nextProcessed->setDisabled(true);

    imageAreaLayout->addLayout(stepLayout, 2, 0);
    slider->setFocus();
}

void Window::nextImg() {
    step++;
    processStep->setText(stepsTitle[static_cast<int>(step - 1)]);

    if(step == 11)
        nextProcessed->setDisabled(true);
    else
        nextProcessed->setEnabled(true);
    prevProcessed->setEnabled(true);

    image->goToImg(static_cast<int>(step));
}

void Window::prevImg() {
    step--;
    processStep->setText(stepsTitle[static_cast<int>(step - 1)]);

    if(step == 1)
        prevProcessed->setDisabled(true);
    else
        prevProcessed->setEnabled(true);
    nextProcessed->setEnabled(true);

    image->goToImg(static_cast<int>(step));
}

void Window::setCirclesNumber(int n) {
    animation->setCirclesNumber(n);
    QString str = tr("Number of circles : ") + QString::number(n);
    sliderValue->setText(str);
}
