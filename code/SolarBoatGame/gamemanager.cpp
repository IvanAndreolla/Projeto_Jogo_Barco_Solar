#include "gamemanager.h"
#include <cstdlib>
#include <cmath>
#include <QDebug>
#include <QFont>

GameManager::GameManager(QGraphicsScene *scene, QGraphicsView *view,
                         QProgressBar *bar, QLabel *labelVel, QLabel *labelVoltas,
                         QPushButton *btn1, QPushButton *btn2, QPushButton *btn3, QPushButton *btnReiniciar,
                         QLabel *titulo, QObject *parent)
    : QObject(parent), scene(scene), view(view),
    barraBateria(bar), labelVelocidade(labelVel), labelVoltas(labelVoltas),
    btnPista1(btn1), btnPista2(btn2), btnPista3(btn3), btnReiniciar(btnReiniciar), labelTitulo(titulo)
{
    inputManager = new InputManager();
    pista = new Pista();
    texturaNuvem.load(":/nuvem.png");

    jogadorLogico = new BarcoJogador(0, 0);
    jogadorVisual = new BarcoGrafico();

    for (int i = 0; i < 3; ++i) {
        BarcoOponente* op = new BarcoOponente(0, 0);
        BarcoGrafico* vis = new BarcoGrafico();
        scene->addItem(vis);
        oponentesLogicos.append(op);
        oponentesVisuais.append(vis);
    }

    connect(btnPista1, &QPushButton::clicked, this, &GameManager::iniciarPista1);
    connect(btnPista2, &QPushButton::clicked, this, &GameManager::iniciarPista2);
    connect(btnPista3, &QPushButton::clicked, this, &GameManager::iniciarPista3);
    connect(btnReiniciar, &QPushButton::clicked, this, &GameManager::reiniciarFase);

    estadoAtual = MENU;
    atualizarInterface();

    scene->setSceneRect(-5000, 0, 10000, 5000);
    criarNuvens();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &GameManager::gameLoop);
    timer->start(16);
}

GameManager::~GameManager() {
    delete jogadorLogico;
    qDeleteAll(oponentesLogicos);
    oponentesLogicos.clear();
    delete inputManager;
    delete pista;
}

void GameManager::processarEvento(QKeyEvent *event, bool pressionado) {
    if (pressionado) inputManager->processarPressaoTecla(event);
    else inputManager->processarSolturaTecla(event);
}

void GameManager::iniciarPista1() { iniciarJogo(Pista::OVAL_FACIL); }
void GameManager::iniciarPista2() { iniciarJogo(Pista::OITO_MEDIO); }
void GameManager::iniciarPista3() { iniciarJogo(Pista::SLALOM_DIFICIL); }
void GameManager::reiniciarFase() { iniciarJogo(pistaAtualSelecionada); }

void GameManager::iniciarJogo(Pista::Tipo tipo) {
    pistaAtualSelecionada = tipo;

    scene->clear();
    listaNuvens.clear();
    qDeleteAll(oponentesLogicos);
    oponentesLogicos.clear();
    oponentesVisuais.clear();

    scene->setSceneRect(-5000, 0, 10000, 5000);
    QPixmap texturaMar(":/mar.png");
    if (texturaMar.isNull()) scene->addRect(-5000, 0, 10000, 5000, Qt::NoPen, QBrush(QColor(0, 105, 148)));
    else scene->addRect(-5000, 0, 10000, 5000, Qt::NoPen, QBrush(texturaMar));

    pista->gerarCenario(scene, tipo);

    Ponto2D start = pista->getPosicaoLargada();
    jogadorLogico->resetar(start.x, start.y);

    jogadorVisual = new BarcoGrafico();
    scene->addItem(jogadorVisual);
    posicaoAnteriorJogador = jogadorLogico->getPosicao();

    for (int i = 0; i < 3; ++i) {
        float offX = (std::rand() % 200 - 100);
        float offY = (std::rand() % 200 - 100);
        BarcoOponente* op = new BarcoOponente(start.x - 200 + offX, start.y + offY);
        BarcoGrafico* vis = new BarcoGrafico();
        scene->addItem(vis);
        oponentesLogicos.append(op);
        oponentesVisuais.append(vis);
    }

    criarNuvens();
    estadoAtual = JOGANDO;
    atualizarInterface();
}

void GameManager::criarNuvens() {
    if (texturaNuvem.isNull()) return;
    for (int i = 0; i < 25; ++i) {
        Nuvem nuvem;
        nuvem.visual = scene->addPixmap(texturaNuvem);
        float escala = (std::rand() % 100 + 200) / 100.0f;
        nuvem.visual->setScale(escala);
        nuvem.visual->setOpacity(0.6);
        nuvem.visual->setZValue(20000);
        int x = std::rand() % 8000 - 2000;
        int y = std::rand() % 8000 - 2000;
        nuvem.visual->setPos(x, y);
        nuvem.velocidade = (std::rand() % 20 + 5) / 10.0f;
        listaNuvens.append(nuvem);
    }
}

void GameManager::atualizarNuvens() {
    bool jogadorNaSombra = false;
    for (Nuvem &nuvem : listaNuvens) {
        nuvem.visual->moveBy(nuvem.velocidade, 0);
        if (nuvem.visual->x() > 8000) {
            nuvem.visual->setX(-2000);
            nuvem.visual->setY(std::rand() % 8000 - 2000);
        }
        if (jogadorVisual && nuvem.visual->collidesWithItem(jogadorVisual)) {
            jogadorNaSombra = true;
        }
    }
    if (jogadorNaSombra) {
        jogadorLogico->setIntensidadeSolar(0.2f);
        jogadorVisual->setOpacity(0.7);
    } else {
        jogadorLogico->setIntensidadeSolar(1.0f);
        jogadorVisual->setOpacity(1.0);
    }
}

void GameManager::atualizarProfundidadeVisual() {
    // Z-Sort usando a BASE da imagem (Bottom Y)
    // sceneBoundingRect().bottom() retorna o Y mais baixo do item na cena.

    if (jogadorVisual) {
        jogadorVisual->setZValue(jogadorVisual->sceneBoundingRect().bottom());
    }

    for (auto* opVis : oponentesVisuais) {
        if (opVis) opVis->setZValue(opVis->sceneBoundingRect().bottom());
    }

    for (QGraphicsItem* item : pista->getObstaculosVisuais()) {
        if (item) {
            // sceneBoundingRect().bottom() para garantir que a base da árvore/ilha seja o critério de profundidade.
            item->setZValue(item->sceneBoundingRect().bottom());
        }
    }
}

void GameManager::fimDeJogo(bool venceu) {
    estadoAtual = GAMEOVER;
    if (venceu) {
        labelTitulo->setText("VITÓRIA!\nCorrida Completada!");
        labelTitulo->setStyleSheet("font-size: 48px; font-weight: bold; color: #00ff00; background-color: rgba(0,0,0,150); padding: 20px; border-radius: 10px;");
    } else {
        labelTitulo->setText("DERROTA\nBateria Esgotada");
        labelTitulo->setStyleSheet("font-size: 48px; font-weight: bold; color: red; background-color: rgba(0,0,0,150); padding: 20px; border-radius: 10px;");
    }
    atualizarInterface();
}

void GameManager::atualizarInterface() {
    if (estadoAtual == MENU) {
        labelTitulo->setText("SOLAR BOAT THE GAME");
        labelTitulo->setStyleSheet("font-size: 60px; font-weight: bold; color: orange; background-color: rgba(0,0,0,100); padding: 20px; border-radius: 10px;");
        labelTitulo->setVisible(true);
        btnPista1->setVisible(true); btnPista2->setVisible(true); btnPista3->setVisible(true);
        btnReiniciar->setVisible(false);
        barraBateria->setVisible(false); labelVelocidade->setVisible(false); labelVoltas->setVisible(false);
    }
    else if (estadoAtual == JOGANDO) {
        labelTitulo->setVisible(false);
        btnPista1->setVisible(false); btnPista2->setVisible(false); btnPista3->setVisible(false);
        btnReiniciar->setVisible(false);
        barraBateria->setVisible(true); labelVelocidade->setVisible(true); labelVoltas->setVisible(true);
    }
    else if (estadoAtual == GAMEOVER) {
        labelTitulo->setVisible(true);
        btnPista1->setVisible(false); btnPista2->setVisible(false); btnPista3->setVisible(false);
        btnReiniciar->setVisible(true);
    }
}

void GameManager::gameLoop() {
    if (estadoAtual == MENU) {
        atualizarNuvens();
        return;
    }

    atualizarNuvens();
    posicaoAnteriorJogador = jogadorLogico->getPosicao();

    jogadorLogico->processarInput(inputManager);
    jogadorLogico->atualizar(0.016f);

    jogadorVisual->atualizarPosicaoTela(jogadorLogico->getPosicao(), jogadorLogico->getAngulo());

    // Z-SORTING CORRIGIDO (Bottom Y)
    atualizarProfundidadeVisual();

    // Colisão
    Ponto2D posObs;
    if (pista->verificarColisaoVisual(jogadorVisual, posObs)) {
        jogadorLogico->chocar(posObs);
        jogadorVisual->atualizarPosicaoTela(jogadorLogico->getPosicao(), jogadorLogico->getAngulo());
    }

    view->centerOn(jogadorVisual);

    if (jogadorLogico->getBateria() <= 0) { fimDeJogo(false); return; }

    const auto& checkpoints = pista->getCheckpoints();
    int idAlvo = jogadorLogico->getProximoCheckpointId();
    if (idAlvo < checkpoints.size()) {
        if (checkpoints[idAlvo]->cruzouLinha(posicaoAnteriorJogador, jogadorLogico->getPosicao())) {
            jogadorLogico->incrementarCheckpoint(checkpoints.size());
            if (jogadorLogico->getVoltaAtual() > 3) { fimDeJogo(true); return; }
        }
    }

    for (int i = 0; i < oponentesLogicos.size(); ++i) {
        oponentesLogicos[i]->tomarDecisao(pista);
        oponentesLogicos[i]->atualizar(0.016f);

        oponentesVisuais[i]->atualizarPosicaoTela(oponentesLogicos[i]->getPosicao(), oponentesLogicos[i]->getAngulo());

        Ponto2D posObsOp;
        if (pista->verificarColisaoVisual(oponentesVisuais[i], posObsOp)) {
            oponentesLogicos[i]->chocar(posObsOp);
        }

        int idAlvoOp = oponentesLogicos[i]->getProximoCheckpointId();
        if (idAlvoOp < checkpoints.size()) {
            if (oponentesLogicos[i]->getPosicao().dist(checkpoints[idAlvoOp]->getCentro()) < 350.0f) {
                oponentesLogicos[i]->incrementarCheckpoint(checkpoints.size());
            }
        }
    }

    barraBateria->setValue((int)jogadorLogico->getBateria());
    if (jogadorLogico->getBateria() < 300) barraBateria->setStyleSheet("QProgressBar::chunk { background-color: red; }");
    else barraBateria->setStyleSheet("QProgressBar::chunk { background-color: #00ff00; }");

    int kmh = (int)(jogadorLogico->getVelocidadeAtual() / 10.0f);
    labelVelocidade->setText(QString("%1 km/h").arg(kmh));

    int v = jogadorLogico->getVoltaAtual();
    if (v == 0) labelVoltas->setText("Posicione para a LARGADA");
    else labelVoltas->setText(QString("Volta: %1/3").arg(v));
}
