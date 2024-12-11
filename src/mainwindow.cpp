#include "mainwindow.h"
#include "settings.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QSettings>
#include <QLabel>
#include <QGroupBox>
#include <QtWidgets/qscrollarea.h>
#include <iostream>

void MainWindow::initialize() {
    realtime = new Realtime;
    aspectRatioWidget = new AspectRatioWidget(this);
    aspectRatioWidget->setAspectWidget(realtime, 3.f/4.f);

    QHBoxLayout *mainLayout = new QHBoxLayout;

    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);

    scrollArea->setFixedWidth(200);

    QWidget *scrollContent = new QWidget(scrollArea);
    QVBoxLayout *vLayout = new QVBoxLayout(scrollContent);
    scrollContent->setLayout(vLayout);
    scrollArea->setWidget(scrollContent);

    QFont font;
    font.setPointSize(12);
    font.setBold(true);

    QLabel *tesselation_label = new QLabel("L System");
    tesselation_label->setFont(font);
    QLabel *camera_label = new QLabel("Camera");
    camera_label->setFont(font);
    QLabel *toon_label = new QLabel("Toon Shading");
    toon_label->setFont(font);
    QLabel *filters_label = new QLabel("Filters");
    filters_label->setFont(font);
    QLabel *ec_label = new QLabel("Extra Credit");
    ec_label->setFont(font);

    QLabel *param1_label = new QLabel("Iteration:");
    QLabel *param2_label = new QLabel("Length");
    QLabel *param3_label = new QLabel("Angle");
    QLabel *param4_label = new QLabel("Day Time");
    QLabel *near_label = new QLabel("Near Plane:");
    QLabel *far_label = new QLabel("Far Plane:");

    filter1 = new QCheckBox("Per-Pixel Filter");
    filter1->setChecked(false);

    filter2 = new QCheckBox("Kernel-Based Filter");
    filter2->setChecked(false);

    saveImage = new QPushButton("Save image");

    QGroupBox *p1Layout = new QGroupBox();
    QHBoxLayout *l1 = new QHBoxLayout();
    QGroupBox *p2Layout = new QGroupBox();
    QHBoxLayout *l2 = new QHBoxLayout();
    QGroupBox *p3Layout = new QGroupBox();
    QHBoxLayout *l3 = new QHBoxLayout();
    QGroupBox *p4Layout = new QGroupBox();
    QHBoxLayout *l4 = new QHBoxLayout();

    p1Slider = new QSlider(Qt::Orientation::Horizontal);
    p1Slider->setTickInterval(1);
    p1Slider->setMinimum(1);
    p1Slider->setMaximum(5);
    p1Slider->setValue(1);

    p1Box = new QSpinBox();
    p1Box->setMinimum(1);
    p1Box->setMaximum(5);
    p1Box->setSingleStep(1);
    p1Box->setValue(1);

    p2Slider = new QSlider(Qt::Orientation::Horizontal);
    p2Slider->setTickInterval(1);
    p2Slider->setMinimum(1);
    p2Slider->setMaximum(25);
    p2Slider->setValue(1);

    p2Box = new QSpinBox();
    p2Box->setMinimum(1);
    p2Box->setMaximum(25);
    p2Box->setSingleStep(1);
    p2Box->setValue(1);

    p3Slider = new QSlider(Qt::Orientation::Horizontal);
    p3Slider->setTickInterval(1);
    p3Slider->setMinimum(1);
    p3Slider->setMaximum(10);
    p3Slider->setValue(1);

    p3Box = new QSpinBox();
    p3Box->setMinimum(1);
    p3Box->setMaximum(10);
    p3Box->setSingleStep(1);
    p3Box->setValue(1);

    p4Slider = new QSlider(Qt::Orientation::Horizontal);
    p4Slider->setTickInterval(1);
    p4Slider->setMinimum(1);
    p4Slider->setMaximum(23);
    p4Slider->setValue(1);

    p4Box = new QSpinBox();
    p4Box->setMinimum(0);
    p4Box->setMaximum(23);
    p4Box->setSingleStep(1);
    p4Box->setValue(0);

    l1->addWidget(p1Slider);
    l1->addWidget(p1Box);
    p1Layout->setLayout(l1);

    l2->addWidget(p2Slider);
    l2->addWidget(p2Box);
    p2Layout->setLayout(l2);

    l3->addWidget(p3Slider);
    l3->addWidget(p3Box);
    p3Layout->setLayout(l3);

    l4->addWidget(p4Slider);
    l4->addWidget(p4Box);
    p4Layout->setLayout(l4);

    QGroupBox *nearLayout = new QGroupBox();
    QHBoxLayout *lnear = new QHBoxLayout();
    QGroupBox *farLayout = new QGroupBox();
    QHBoxLayout *lfar = new QHBoxLayout();

    nearSlider = new QSlider(Qt::Orientation::Horizontal);
    nearSlider->setTickInterval(1);
    nearSlider->setMinimum(1);
    nearSlider->setMaximum(1000);
    nearSlider->setValue(10);

    nearBox = new QDoubleSpinBox();
    nearBox->setMinimum(0.01f);
    nearBox->setMaximum(10.f);
    nearBox->setSingleStep(0.1f);
    nearBox->setValue(0.1f);

    farSlider = new QSlider(Qt::Orientation::Horizontal);
    farSlider->setTickInterval(1);
    farSlider->setMinimum(1000);
    farSlider->setMaximum(10000);
    farSlider->setValue(10000);

    farBox = new QDoubleSpinBox();
    farBox->setMinimum(10.f);
    farBox->setMaximum(100.f);
    farBox->setSingleStep(0.1f);
    farBox->setValue(100.f);

    lnear->addWidget(nearSlider);
    lnear->addWidget(nearBox);
    nearLayout->setLayout(lnear);

    lfar->addWidget(farSlider);
    lfar->addWidget(farBox);
    farLayout->setLayout(lfar);

    QGroupBox *t1Layout = new QGroupBox();
    QHBoxLayout *t1 = new QHBoxLayout();

    t1Slider = new QSlider(Qt::Orientation::Horizontal);
    t1Slider->setTickInterval(1);
    t1Slider->setMinimum(1);
    t1Slider->setMaximum(10);
    t1Slider->setValue(1);

    t1Box = new QSpinBox();
    t1Box->setMinimum(1);
    t1Box->setMaximum(10);
    t1Box->setSingleStep(1);
    t1Box->setValue(1);

    t1->addWidget(t1Slider);
    t1->addWidget(t1Box);
    t1Layout->setLayout(t1);
    tBool = new QCheckBox("Enable Toon Shading");
    tBool->setChecked(false);

    // Extra Credit
    ec1 = new QCheckBox("Shadow Map");
    ec1->setChecked(false);

    ec2 = new QCheckBox("Forest");
    ec2->setChecked(false);

    ec3 = new QCheckBox("Snow");
    ec3->setChecked(false);

    ec4 = new QCheckBox("Leaf");
    ec4->setChecked(false);

    vLayout->addWidget(saveImage);
    vLayout->addWidget(tesselation_label);
    vLayout->addWidget(param1_label);
    vLayout->addWidget(p1Layout);
    vLayout->addWidget(param2_label);
    vLayout->addWidget(p2Layout);
    vLayout->addWidget(param3_label);
    vLayout->addWidget(p3Layout);
    vLayout->addWidget(param4_label);
    vLayout->addWidget(p4Layout);
    vLayout->addWidget(camera_label);
    vLayout->addWidget(near_label);
    vLayout->addWidget(nearLayout);
    vLayout->addWidget(far_label);
    vLayout->addWidget(farLayout);
    vLayout->addWidget(filters_label);
    vLayout->addWidget(filter1);
    vLayout->addWidget(filter2);
    vLayout->addWidget(toon_label);
    vLayout->addWidget(tBool);
    vLayout->addWidget(t1Layout);
    vLayout->addWidget(ec_label);
    vLayout->addWidget(ec1);
    vLayout->addWidget(ec2);
    vLayout->addWidget(ec3);
    vLayout->addWidget(ec4);

    mainLayout->addWidget(scrollArea, 1);
    mainLayout->addWidget(aspectRatioWidget, 3);

    this->setLayout(mainLayout);

    connectUIElements();

    // set default values of 5 for tesselation parameters
    onValChangeP1(1);
    onValChangeP2(1);
    onValChangeP3(1);
    onValChangeP4(1);

    onValChangeT1(0);

    // Set default values for near and far planes
    onValChangeNearBox(0.1f);
    onValChangeFarBox(100.f);


}


void MainWindow::finish() {
    realtime->finish();
    delete(realtime);
}

void MainWindow::connectUIElements() {
    connectPerPixelFilter();
    connectKernelBasedFilter();
   // connectUploadFile();
    // connectLSystemGenerate();
    connectSaveImage();
    connectParam1();
    connectParam2();
    connectParam3();
    connectParam4();
    connectNear();
    connectFar();
    connect(tBool, &QCheckBox::clicked, this, &MainWindow::onToonEnable);
    connectToonSlider();
    connectExtraCredit();
}

void MainWindow::connectPerPixelFilter() {
    connect(filter1, &QCheckBox::clicked, this, &MainWindow::onPerPixelFilter);
}

void MainWindow::connectKernelBasedFilter() {
    connect(filter2, &QCheckBox::clicked, this, &MainWindow::onKernelBasedFilter);
}

void MainWindow::connectSaveImage() {
    connect(saveImage, &QPushButton::clicked, this, &MainWindow::onSaveImage);
}

void MainWindow::connectParam1() {
    connect(p1Slider, &QSlider::valueChanged, this, &MainWindow::onValChangeP1);
    connect(p1Box, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &MainWindow::onValChangeP1);
}

void MainWindow::connectParam2() {
    connect(p2Slider, &QSlider::valueChanged, this, &MainWindow::onValChangeP2);
    connect(p2Box, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &MainWindow::onValChangeP2);
}

void MainWindow::connectParam3() {
    connect(p3Slider, &QSlider::valueChanged, this, &MainWindow::onValChangeP3);
    connect(p3Box, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &MainWindow::onValChangeP3);
}

void MainWindow::connectParam4() {
    connect(p4Slider, &QSlider::valueChanged, this, &MainWindow::onValChangeP4);
    connect(p4Box, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &MainWindow::onValChangeP4);
}

void MainWindow::connectNear() {
    connect(nearSlider, &QSlider::valueChanged, this, &MainWindow::onValChangeNearSlider);
    connect(nearBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onValChangeNearBox);
}

void MainWindow::connectFar() {
    connect(farSlider, &QSlider::valueChanged, this, &MainWindow::onValChangeFarSlider);
    connect(farBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onValChangeFarBox);
}

void MainWindow::connectToonSlider() {
    connect(t1Slider, &QSlider::valueChanged, this, &MainWindow::onValChangeT1);
    connect(t1Box, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &MainWindow::onValChangeT1);
}

void MainWindow::connectExtraCredit() {
    connect(ec1, &QCheckBox::clicked, this, &MainWindow::onExtraCredit1);
    connect(ec2, &QCheckBox::clicked, this, &MainWindow::onExtraCredit2);
    connect(ec3, &QCheckBox::clicked, this, &MainWindow::onExtraCredit3);
    connect(ec4, &QCheckBox::clicked, this, &MainWindow::onExtraCredit4);
}

void MainWindow::onPerPixelFilter() {
    settings.perPixelFilter = !settings.perPixelFilter;
    realtime->settingsChanged();
}

void MainWindow::onKernelBasedFilter() {
    settings.kernelBasedFilter = !settings.kernelBasedFilter;
    realtime->settingsChanged();
}

// void MainWindow::onUploadFile() {
//     // Get abs path of scene file
//     QString configFilePath = QFileDialog::getOpenFileName(this, tr("Upload File"),
//                                                           QDir::currentPath()
//                                                               .append(QDir::separator())
//                                                               .append("scenefiles")
//                                                               .append(QDir::separator())
//                                                               .append("action")
//                                                               .append(QDir::separator())
//                                                               .append("required"), tr("Scene Files (*.json)"));
//     if (configFilePath.isNull()) {
//         std::cout << "Failed to load null scenefile." << std::endl;
//         return;
//     }

//     settings.sceneFilePath = configFilePath.toStdString();

//     std::cout << "Loaded scenefile: \"" << configFilePath.toStdString() << "\"." << std::endl;

//     realtime->sceneChanged();
// }

void MainWindow::onLSystem() {
    realtime->lSystemGeneration();
}

void MainWindow::onSaveImage() {
    std::string sceneName = settings.sceneFilePath.substr(0, settings.sceneFilePath.find_last_of("."));
    sceneName = sceneName.substr(sceneName.find_last_of("/")+1);
    QString filePath = QFileDialog::getSaveFileName(this, tr("Save Image"),
                                                    QDir::currentPath()
                                                        .append(QDir::separator())
                                                        .append("output")
                                                        .append(QDir::separator())
                                                        .append(sceneName), tr("Image Files (*.png)"));
    std::cout << "Saving image to: \"" << filePath.toStdString() << "\"." << std::endl;
    realtime->saveViewportImage(filePath.toStdString());
}

void MainWindow::onValChangeP1(int newValue) {
    p1Slider->setValue(newValue);
    p1Box->setValue(newValue);
    settings.shapeParameter1 = p1Slider->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeP2(int newValue) {
    p2Slider->setValue(newValue);
    p2Box->setValue(newValue);
    settings.shapeParameter2 = p2Slider->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeP3(int newValue) {
    p3Slider->setValue(newValue);
    p3Box->setValue(newValue);
    settings.shapeParameter3 = p3Slider->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeP4(int newValue) {
    p4Slider->setValue(newValue);
    p4Box->setValue(newValue);
    settings.shapeParameter4 = p4Slider->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeNearSlider(int newValue) {
    //nearSlider->setValue(newValue);
    nearBox->setValue(newValue/100.f);
    settings.nearPlane = nearBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeFarSlider(int newValue) {
    //farSlider->setValue(newValue);
    farBox->setValue(newValue/100.f);
    settings.farPlane = farBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeNearBox(double newValue) {
    nearSlider->setValue(int(newValue*100.f));
    //nearBox->setValue(newValue);
    settings.nearPlane = nearBox->value();
    realtime->settingsChanged();
}

void MainWindow::onValChangeFarBox(double newValue) {
    farSlider->setValue(int(newValue*100.f));
    //farBox->setValue(newValue);
    settings.farPlane = farBox->value();
    realtime->settingsChanged();
}

void MainWindow::onToonEnable() {
    settings.toonEnable = !settings.toonEnable;
    realtime->settingsChanged();
}

void MainWindow::onValChangeT1(int newValue) {
    t1Slider->setValue(newValue);
    t1Box->setValue(newValue);
    settings.toonLevel = t1Slider->value();
    realtime->settingsChanged();
}

// Extra Credit:

void MainWindow::onExtraCredit1() {
    settings.extraCredit1 = !settings.extraCredit1;
    realtime->settingsChanged();
}

void MainWindow::onExtraCredit2() {
    settings.extraCredit2 = !settings.extraCredit2;
    realtime->settingsChanged();
}

void MainWindow::onExtraCredit3() {
    settings.extraCredit3 = !settings.extraCredit3;
    realtime->settingsChanged();
}

void MainWindow::onExtraCredit4() {
    settings.extraCredit4 = !settings.extraCredit4;
    realtime->settingsChanged();
}
