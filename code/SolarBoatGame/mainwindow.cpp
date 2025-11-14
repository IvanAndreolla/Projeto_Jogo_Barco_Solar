#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // --- Configuração Visual (VIEW) ---
    scene = new QGraphicsScene(this);
    scene->setSceneRect(-5000, 0, 10000, 5000);

    // FUNDO: Textura do Mar
    QPixmap texturaMar(":/mar.png");
    if (texturaMar.isNull()) {
        // Fallback: Cor azul sólida se a imagem falhar
        scene->addRect(-5000, 0, 10000, 5000, Qt::NoPen, QBrush(QColor(0, 105, 148)));
    } else {
        // Pincel de textura (repete a imagem)
        QBrush brushMar(texturaMar);
        scene->addRect(-5000, 0, 10000, 5000, Qt::NoPen, brushMar);
    }

    view = new QGraphicsView(scene, this);
    view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // --- Configuração HUD ---
    QWidget *hudWidget = new QWidget(this);
    hudWidget->setAttribute(Qt::WA_TransparentForMouseEvents);
    QVBoxLayout *layout = new QVBoxLayout(hudWidget);

    labelVelocidade = new QLabel("0 km/h", hudWidget);
    labelVelocidade->setStyleSheet("font-size: 24px; font-weight: bold; color: white; text-shadow: 1px 1px 0 #000;");
    layout->addWidget(labelVelocidade);

    labelVoltas = new QLabel("Volta: 1", hudWidget);
    labelVoltas->setStyleSheet("font-size: 18px; font-weight: bold; color: yellow; text-shadow: 1px 1px 0 #000;");
    layout->addWidget(labelVoltas);

    barraBateria = new QProgressBar(hudWidget);
    barraBateria->setRange(0, 1000);
    barraBateria->setValue(1000);
    barraBateria->setStyleSheet("QProgressBar { border: 2px solid grey; border-radius: 5px; background-color: #333; color: white; } QProgressBar::chunk { background-color: #00ff00; }");
    layout->addWidget(barraBateria);
    layout->addStretch();

    setCentralWidget(view);
    hudWidget->resize(200, 120);
    hudWidget->move(20, 20);
    hudWidget->raise();

    // --- INICIALIZAÇÃO DO JOGO ---
    gameManager = new GameManager(scene, view, barraBateria, labelVelocidade, labelVoltas, this);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if(gameManager) gameManager->processarEvento(event, true);
}

void MainWindow::keyReleaseEvent(QKeyEvent *event) {
    if(gameManager) gameManager->processarEvento(event, false);
}
