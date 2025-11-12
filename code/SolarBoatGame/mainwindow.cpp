#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // --- 1. CONFIGURAÇÃO GRÁFICA ---
    scene = new QGraphicsScene(this);
    view = new QGraphicsView(scene, this);

    // Define o tamanho do "mundo" (800x600 pixels)
    scene->setSceneRect(0, 0, 800, 600);

    // Faz a câmera (view) ocupar toda a janela
    setCentralWidget(view);
    resize(820, 620); // Ajusta o tamanho da janela

    // Criar um "avatar" temporário para o barco (Quadrado Azul de 20x20)
    barcoVisual = scene->addRect(-10, -10, 20, 20, QPen(Qt::black), QBrush(Qt::blue));
    barcoVisual->setTransformOriginPoint(0, 0); // Para rodar pelo centro

    // --- 2. INICIALIZAÇÃO LÓGICA ---
    // Inicia o barco lógico no centro da tela
    barco = new BarcoSolar(400, 300);
    frameCount = 0;

    // --- 3. LOOP DO JOGO ---
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::atualizarJogo);
    timer->start(16); // ~60 FPS
}

MainWindow::~MainWindow()
{
    delete ui;
    delete barco;
}

void MainWindow::atualizarJogo() {
    frameCount++;

    // --- SIMULAÇÃO DE INPUT (A mesma de antes) ---
    if (frameCount < 100) {
        barco->acelerar();
        // Vamos fazer ele girar um pouco também para testar a rotação
        barco->virar(1.0f);
    }

    // --- ATUALIZAÇÃO DA FÍSICA ---
    barco->atualizar(0.016f);

    // --- ATUALIZAÇÃO DOS GRÁFICOS (A Mágica Acontece Aqui!) ---

    // 1. Pegamos a posição calculada pela física
    Ponto2D pos = barco->getPosicao();
    float angulo = barco->getAngulo();

    // 2. Aplicamos ao objeto visual na tela
    barcoVisual->setPos(pos.x, pos.y);
    barcoVisual->setRotation(angulo);

    // Debug no console (opcional, para conferir)
    if (frameCount % 60 == 0) { // Só imprime a cada 1 segundo para não poluir
        qDebug() << "Pos:" << pos.x << "," << pos.y << "Ang:" << angulo;
    }
}
