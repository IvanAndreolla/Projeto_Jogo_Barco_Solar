#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Visual
    scene = new QGraphicsScene(this);
    scene->setSceneRect(-5000, 0, 10000, 5000);

    QPixmap texturaMar(":/mar.png");
    if (texturaMar.isNull()) scene->addRect(-5000, 0, 10000, 5000, Qt::NoPen, QBrush(QColor(0, 105, 148)));
    else scene->addRect(-5000, 0, 10000, 5000, Qt::NoPen, QBrush(texturaMar));

    view = new QGraphicsView(scene, this);
    view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // UI Layer
    uiLayer = new QWidget(this);
    QVBoxLayout *menuLayout = new QVBoxLayout(uiLayer);
    menuLayout->setAlignment(Qt::AlignCenter);

    labelTitulo = new QLabel("SOLAR BOAT THE GAME", uiLayer);
    labelTitulo->setAlignment(Qt::AlignCenter);
    menuLayout->addWidget(labelTitulo);

    QString btnStyle = "QPushButton { background-color: #ff8c00; color: white; font-weight: bold; font-size: 20px; padding: 10px; border-radius: 10px; min-width: 200px; margin: 5px; } QPushButton:hover { background-color: #ffaa33; }";

    // Botões de Pista
    btnPista1 = new QPushButton("PISTA 1: OVAL", uiLayer);
    btnPista1->setCursor(Qt::PointingHandCursor);
    btnPista1->setStyleSheet(btnStyle);
    menuLayout->addWidget(btnPista1);

    btnPista2 = new QPushButton("PISTA 2: OITO", uiLayer);
    btnPista2->setCursor(Qt::PointingHandCursor);
    btnPista2->setStyleSheet(btnStyle);
    menuLayout->addWidget(btnPista2);

    btnPista3 = new QPushButton("PISTA 3: SLALOM", uiLayer);
    btnPista3->setCursor(Qt::PointingHandCursor);
    btnPista3->setStyleSheet(btnStyle);
    menuLayout->addWidget(btnPista3);

    btnReiniciar = new QPushButton("JOGAR NOVAMENTE", uiLayer);
    btnReiniciar->setCursor(Qt::PointingHandCursor);
    btnReiniciar->setStyleSheet(btnStyle);
    btnReiniciar->setVisible(false);
    menuLayout->addWidget(btnReiniciar);

    menuLayout->addStretch();

    // HUD
    labelVelocidade = new QLabel("0 km/h", uiLayer);
    labelVelocidade->setStyleSheet("font-size: 24px; font-weight: bold; color: white; text-shadow: 1px 1px 0 #000;");
    labelVelocidade->move(20, 20);

    labelVoltas = new QLabel("Volta: 1", uiLayer);
    labelVoltas->setStyleSheet("font-size: 18px; font-weight: bold; color: yellow; text-shadow: 1px 1px 0 #000;");
    labelVoltas->move(20, 50);

    barraBateria = new QProgressBar(uiLayer);
    barraBateria->setRange(0, 1000);
    barraBateria->setValue(1000);
    barraBateria->setStyleSheet("QProgressBar { border: 2px solid grey; border-radius: 5px; background-color: #333; color: white; } QProgressBar::chunk { background-color: #00ff00; }");
    barraBateria->resize(200, 25);
    barraBateria->move(20, 80);

    setCentralWidget(view);
    uiLayer->resize(1024, 768);
    uiLayer->raise();

    gameManager = new GameManager(scene, view, barraBateria, labelVelocidade, labelVoltas,
                                  btnPista1, btnPista2, btnPista3, btnReiniciar, labelTitulo, this);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    if (uiLayer) uiLayer->resize(event->size());
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if(gameManager) gameManager->processarEvento(event, true);
}

void MainWindow::keyReleaseEvent(QKeyEvent *event) {
    if(gameManager) gameManager->processarEvento(event, false);
}
