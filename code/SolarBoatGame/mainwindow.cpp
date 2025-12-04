#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QVBoxLayout>
#include <QFrame>

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

    labelTitulo = new QLabel("SOLAR BOAT RACING", uiLayer);
    labelTitulo->setAlignment(Qt::AlignCenter);
    menuLayout->addWidget(labelTitulo);

    QString btnStyle = "QPushButton { background-color: #ff8c00; color: white; font-weight: bold; font-size: 20px; padding: 10px; border-radius: 10px; min-width: 200px; margin: 5px; } QPushButton:hover { background-color: #ffaa33; }";
    QString btnGreen = "QPushButton { background-color: #44aa44; color: white; font-weight: bold; font-size: 20px; padding: 10px; border-radius: 10px; min-width: 200px; margin: 5px; }";
    QString btnRed = "QPushButton { background-color: #cc4444; color: white; font-weight: bold; font-size: 20px; padding: 10px; border-radius: 10px; min-width: 200px; margin: 5px; }";

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

    btnRetomar = new QPushButton("RETOMAR", uiLayer);
    btnRetomar->setCursor(Qt::PointingHandCursor);
    btnRetomar->setStyleSheet(btnGreen);
    btnRetomar->setVisible(false);
    menuLayout->addWidget(btnRetomar);

    btnReiniciar = new QPushButton("REINICIAR PISTA", uiLayer);
    btnReiniciar->setCursor(Qt::PointingHandCursor);
    btnReiniciar->setStyleSheet(btnStyle);
    btnReiniciar->setVisible(false);
    menuLayout->addWidget(btnReiniciar);

    btnMenuPrincipal = new QPushButton("MENU PRINCIPAL", uiLayer);
    btnMenuPrincipal->setCursor(Qt::PointingHandCursor);
    btnMenuPrincipal->setStyleSheet(btnRed);
    btnMenuPrincipal->setVisible(false);
    menuLayout->addWidget(btnMenuPrincipal);

    menuLayout->addStretch();

    // --- PAINEL HUD ---
    painelHUD = new QFrame(uiLayer);
    painelHUD->setGeometry(20, 20, 250, 280);

    painelHUD->setStyleSheet(
        "QFrame { background-color: rgba(0, 0, 0, 180); border: 2px solid #444; border-radius: 10px; }"
        "QLabel { color: white; font-weight: bold; border: none; background: transparent; }"
        );

    QVBoxLayout *hudLayout = new QVBoxLayout(painelHUD);
    hudLayout->setContentsMargins(15, 15, 15, 15);
    hudLayout->setSpacing(5);

    labelVelocidade = new QLabel("0 km/h", painelHUD);
    labelVelocidade->setStyleSheet("font-size: 28px; color: #00ffff;");
    labelVelocidade->setAlignment(Qt::AlignCenter);
    hudLayout->addWidget(labelVelocidade);

    barraBateria = new QProgressBar(painelHUD);
    barraBateria->setRange(0, 1000);
    barraBateria->setValue(1000);
    barraBateria->setTextVisible(false);
    barraBateria->setFixedHeight(10);
    barraBateria->setStyleSheet("QProgressBar { background-color: #333; border-radius: 5px; border: none; } QProgressBar::chunk { background-color: #00ff00; border-radius: 5px; }");
    hudLayout->addWidget(barraBateria);

    QLabel* lblBatTitle = new QLabel("BATERIA", painelHUD);
    lblBatTitle->setStyleSheet("font-size: 10px; color: #aaa;");
    lblBatTitle->setAlignment(Qt::AlignCenter);
    hudLayout->addWidget(lblBatTitle);

    QFrame* line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setStyleSheet("background-color: #666; max-height: 1px;");
    hudLayout->addWidget(line);

    // Voltas
    labelVoltas = new QLabel("Volta 1/3", painelHUD);
    labelVoltas->setStyleSheet("font-size: 14px; color: yellow;");
    labelVoltas->setAlignment(Qt::AlignCenter);
    hudLayout->addWidget(labelVoltas);

    // Ranking
    labelPosicao = new QLabel("RANKING:\n1. JOGADOR\n2. CPU 1\n3. CPU 2\n4. CPU 3", painelHUD);
    labelPosicao->setStyleSheet("font-size: 14px; color: white;");
    labelPosicao->setAlignment(Qt::AlignLeft);
    hudLayout->addWidget(labelPosicao);

    setCentralWidget(view);
    uiLayer->resize(1024, 768);
    uiLayer->raise();

    gameManager = new GameManager(scene, view, painelHUD,
                                  barraBateria, labelVelocidade, labelVoltas, labelPosicao,
                                  btnPista1, btnPista2, btnPista3, btnReiniciar, btnRetomar, btnMenuPrincipal,
                                  labelTitulo, this);
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
