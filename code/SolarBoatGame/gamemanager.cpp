#include "gamemanager.h"
#include <cstdlib>
#include <cmath>
#include <QDebug>
#include <QFont>
#include <algorithm>

GameManager::GameManager(QGraphicsScene *scene, QGraphicsView *view,
                         QFrame *painel,
                         QProgressBar *bar, QLabel *labelVel, QLabel *labelVoltas, QLabel *labelPos,
                         QPushButton *btn1, QPushButton *btn2, QPushButton *btn3, QPushButton *btnReiniciar,
                         QPushButton *btnRetomar, QPushButton *btnMenuPrincipal,
                         QLabel *titulo, QObject *parent)
    : QObject(parent), scene(scene), view(view),
    painelHUD(painel),
    barraBateria(bar), labelVelocidade(labelVel), labelVoltas(labelVoltas), labelPosicao(labelPos),
    btnPista1(btn1), btnPista2(btn2), btnPista3(btn3), btnReiniciar(btnReiniciar),
    btnRetomar(btnRetomar), btnMenuPrincipal(btnMenuPrincipal),
    labelTitulo(titulo)
{
    inputManager = new InputManager();
    pista = new Pista();
    texturaNuvem.load(":/nuvem.png");

    modoDebug = false; // Debug começa desligado
    jogadorLogico = new BarcoJogador(0, 0, "JOGADOR");
    jogadorVisual = new BarcoGrafico();
    todosBarcos.append(jogadorLogico);

    for (int i = 0; i < 3; ++i) {
        BarcoOponente* op = new BarcoOponente(0, 0, QString("CPU %1").arg(i+1));
        BarcoGrafico* vis = new BarcoGrafico();
        scene->addItem(vis); // Adiciona para não crashar, mas limpa no iniciar
        oponentesLogicos.append(op);
        oponentesVisuais.append(vis);
        posicoesAnterioresOponentes.append(Ponto2D(0,0));
        todosBarcos.append(op);
    }

    connect(btnPista1, &QPushButton::clicked, this, &GameManager::iniciarPista1);
    connect(btnPista2, &QPushButton::clicked, this, &GameManager::iniciarPista2);
    connect(btnPista3, &QPushButton::clicked, this, &GameManager::iniciarPista3);
    connect(btnReiniciar, &QPushButton::clicked, this, &GameManager::reiniciarFase);
    connect(btnRetomar, &QPushButton::clicked, this, &GameManager::retomarJogo);
    connect(btnMenuPrincipal, &QPushButton::clicked, this, &GameManager::voltarAoMenu);

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
    if (pressionado) {
        if (event->key() == Qt::Key_Escape) {
            if (estadoAtual == JOGANDO) pausarJogo();
            else if (estadoAtual == PAUSADO) retomarJogo();
        }
        // Tecla F1: Alterna Debug para TODOS
        if (event->key() == Qt::Key_F1) {
            modoDebug = !modoDebug;
            if (jogadorVisual) jogadorVisual->setDebug(modoDebug);
            for (auto* vis : oponentesVisuais) if (vis) vis->setDebug(modoDebug);
        }
    }
    if (pressionado) inputManager->processarPressaoTecla(event);
    else inputManager->processarSolturaTecla(event);
}

void GameManager::iniciarPista1() { iniciarJogo(Pista::OVAL_FACIL); }
void GameManager::iniciarPista2() { iniciarJogo(Pista::OITO_MEDIO); }
void GameManager::iniciarPista3() { iniciarJogo(Pista::SLALOM_DIFICIL); }
void GameManager::reiniciarFase() { iniciarJogo(pistaAtualSelecionada); }

void GameManager::pausarJogo() {
    estadoAtual = PAUSADO;
    timer->stop();
    labelTitulo->setText("JOGO PAUSADO");
    atualizarInterface();
}

void GameManager::retomarJogo() {
    estadoAtual = JOGANDO;
    timer->start(16);
    atualizarInterface();
}

void GameManager::voltarAoMenu() {
    estadoAtual = MENU;
    scene->clear();
    listaNuvens.clear();
    oponentesVisuais.clear();
    todosBarcos.clear();
    barcosFinalizados.clear();
    posicoesAnterioresOponentes.clear();

    scene->setSceneRect(-5000, 0, 10000, 5000);
    QPixmap texturaMar(":/mar.png");
    if (texturaMar.isNull()) scene->addRect(-5000, 0, 10000, 5000, Qt::NoPen, QBrush(QColor(0, 105, 148)));
    else scene->addRect(-5000, 0, 10000, 5000, Qt::NoPen, QBrush(texturaMar));

    jogadorVisual = new BarcoGrafico();
    criarNuvens();
    timer->start(16);
    atualizarInterface();
}

void GameManager::iniciarJogo(Pista::Tipo tipo) {
    pistaAtualSelecionada = tipo;
    scene->clear();
    listaNuvens.clear();
    oponentesVisuais.clear();
    todosBarcos.clear();
    barcosFinalizados.clear();
    posicoesAnterioresOponentes.clear();

    scene->setSceneRect(-5000, 0, 10000, 5000);
    QPixmap texturaMar(":/mar.png");
    if (texturaMar.isNull()) scene->addRect(-5000, 0, 10000, 5000, Qt::NoPen, QBrush(QColor(0, 105, 148)));
    else scene->addRect(-5000, 0, 10000, 5000, Qt::NoPen, QBrush(texturaMar));

    pista->gerarCenario(scene, tipo);
    Ponto2D start = pista->getPosicaoLargada();

    // Grid de Largada
    float espX = 150.0f;
    float espY = 120.0f;

    // Jogador
    jogadorLogico->resetar(start.x - espX, start.y);
    todosBarcos.append(jogadorLogico);
    jogadorVisual = new BarcoGrafico();
    jogadorVisual->setDebug(modoDebug); // Respeita estado atual
    scene->addItem(jogadorVisual);
    posicaoAnteriorJogador = jogadorLogico->getPosicao();

    // Oponentes
    oponentesLogicos[0]->resetar(start.x + espX, start.y);
    oponentesLogicos[1]->resetar(start.x - espX, start.y + espY);
    oponentesLogicos[2]->resetar(start.x + espX, start.y + espY);

    for (int i = 0; i < 3; ++i) {
        BarcoGrafico* vis = new BarcoGrafico();
        vis->setDebug(modoDebug); // Respeita estado atual
        scene->addItem(vis);
        oponentesVisuais.append(vis);
        todosBarcos.append(oponentesLogicos[i]);
        posicoesAnterioresOponentes.append(oponentesLogicos[i]->getPosicao());
    }

    criarNuvens();
    estadoAtual = JOGANDO;
    atualizarInterface();
    if (!timer->isActive()) timer->start(16);
    view->centerOn(jogadorVisual);
}

void GameManager::criarNuvens() {
    if (texturaNuvem.isNull()) return;
    for (int i = 0; i < 15; ++i) {
        Nuvem nuvem;
        nuvem.visual = scene->addPixmap(texturaNuvem);
        float escala = (std::rand() % 100 + 50) / 100.0f;
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
        if (jogadorVisual && jogadorVisual->scene() == scene && nuvem.visual->collidesWithItem(jogadorVisual)) {
            jogadorNaSombra = true;
        }
    }
    if (jogadorNaSombra) {
        jogadorLogico->setIntensidadeSolar(0.2f);
        if (jogadorVisual) jogadorVisual->setOpacity(0.7);
    } else {
        jogadorLogico->setIntensidadeSolar(1.0f);
        if (jogadorVisual) jogadorVisual->setOpacity(1.0);
    }
}

void GameManager::atualizarProfundidadeVisual() {
    if (jogadorVisual) jogadorVisual->setZValue(jogadorVisual->sceneBoundingRect().bottom());
    for (auto* opVis : oponentesVisuais) if (opVis) opVis->setZValue(opVis->sceneBoundingRect().bottom());
    for (QGraphicsItem* item : pista->getObstaculosVisuais()) if (item) item->setZValue(item->sceneBoundingRect().bottom());
}

void GameManager::atualizarRanking() {
    QList<BarcoSolar*> ranking = todosBarcos;
    std::sort(ranking.begin(), ranking.end(), [](BarcoSolar* a, BarcoSolar* b) {
        if (a->isTerminado() && !b->isTerminado()) return true;
        if (!a->isTerminado() && b->isTerminado()) return false;
        return a->getScoreCorrida() > b->getScoreCorrida();
    });
    QString textoPlacar = "RANKING:\n";
    for (int i = 0; i < ranking.size(); ++i) {
        QString nome = ranking[i]->getNome();
        if (todosBarcos[i] == jogadorLogico) nome += " (VOCÊ)";
        textoPlacar += QString("%1. %2\n").arg(i+1).arg(nome);
    }
    labelPosicao->setText(textoPlacar);
}

void GameManager::fimDeJogo(bool venceu) {
    estadoAtual = GAMEOVER;
    timer->stop();
    atualizarRanking();
    if (venceu) {
        labelTitulo->setText("CORRIDA FINALIZADA!");
        labelTitulo->setStyleSheet("font-size: 48px; font-weight: bold; color: #00ff00; background-color: rgba(0,0,0,150); padding: 20px; border-radius: 10px;");
    } else {
        labelTitulo->setText("DERROTA");
        labelTitulo->setStyleSheet("font-size: 48px; font-weight: bold; color: red; background-color: rgba(0,0,0,150); padding: 20px; border-radius: 10px;");
    }
    atualizarInterface();
}

void GameManager::atualizarInterface() {
    labelTitulo->setVisible(false);
    btnPista1->setVisible(false); btnPista2->setVisible(false); btnPista3->setVisible(false);
    btnReiniciar->setVisible(false); btnRetomar->setVisible(false); btnMenuPrincipal->setVisible(false);
    painelHUD->setVisible(false);

    if (estadoAtual == MENU) {
        labelTitulo->setText("SOLAR BOAT RACING");
        labelTitulo->setStyleSheet("font-size: 60px; font-weight: bold; color: orange; background-color: rgba(0,0,0,100); padding: 20px; border-radius: 10px;");
        labelTitulo->setVisible(true);
        btnPista1->setVisible(true); btnPista2->setVisible(true); btnPista3->setVisible(true);
    }
    else if (estadoAtual == JOGANDO) {
        painelHUD->setVisible(true);
    }
    else if (estadoAtual == PAUSADO) {
        labelTitulo->setVisible(true);
        labelTitulo->setStyleSheet("font-size: 48px; font-weight: bold; color: white; background-color: rgba(0,0,0,150); padding: 20px; border-radius: 10px;");
        btnRetomar->setVisible(true);
        btnReiniciar->setVisible(true);
        btnMenuPrincipal->setVisible(true);
        painelHUD->setVisible(true);
    }
    else if (estadoAtual == GAMEOVER) {
        labelTitulo->setVisible(true);
        btnReiniciar->setVisible(true);
        btnMenuPrincipal->setVisible(true);
        painelHUD->setVisible(true);
    }
}

void GameManager::gameLoop() {
    if (estadoAtual == MENU || estadoAtual == PAUSADO) {
        atualizarNuvens();
        return;
    }

    atualizarNuvens();
    jogadorLogico->limparDebug();

    posicaoAnteriorJogador = jogadorLogico->getPosicao();
    for(int i=0; i<oponentesLogicos.size(); i++) posicoesAnterioresOponentes[i] = oponentesLogicos[i]->getPosicao();

    if (!jogadorLogico->isTerminado()) {
        jogadorLogico->processarInput(inputManager);
        jogadorLogico->atualizar(0.016f);

        // Debug Jogador
        jogadorVisual->atualizarDebugInfo(
            jogadorLogico->getDebugForcaMotor(),
            jogadorLogico->getDebugForcaArrasto(),
            jogadorLogico->getDebugForcaResultante(),
            jogadorLogico->getDebugAnguloLeme()
            );

        Ponto2D posObs;
        if (pista->verificarColisaoVisual(jogadorVisual, posObs)) jogadorLogico->chocar(posObs);

        const auto& checkpoints = pista->getCheckpoints();
        int idAlvo = jogadorLogico->getProximoCheckpointId();
        if (idAlvo < checkpoints.size()) {
            if (checkpoints[idAlvo]->cruzouLinha(posicaoAnteriorJogador, jogadorLogico->getPosicao())) {
                jogadorLogico->incrementarCheckpoint(checkpoints.size());
                if (jogadorLogico->getVoltaAtual() > 3) {
                    jogadorLogico->setTerminado(true);
                    barcosFinalizados.append(jogadorLogico);
                }
            }
        }
        if (jogadorLogico->getBateria() <= 0) { fimDeJogo(false); return; }
    } else {
        // Se terminou, apenas atualiza inércia
        jogadorLogico->atualizar(0.016f);
        // Atualiza visual do debug (sem forças) para o barco não "congelar" as setas
        jogadorVisual->atualizarDebugInfo(Ponto2D(0,0), Ponto2D(0,0), Ponto2D(0,0), 0);
    }

    jogadorVisual->atualizarPosicaoTela(jogadorLogico->getPosicao(), jogadorLogico->getAngulo());
    atualizarProfundidadeVisual();

    // Colisões Barco x Barco
    for (int i = 0; i < oponentesVisuais.size(); ++i) {
        if (jogadorVisual->collidesWithItem(oponentesVisuais[i], Qt::IntersectsItemShape)) {
            jogadorLogico->colidirComBarco(oponentesLogicos[i]);
            oponentesLogicos[i]->colidirComBarco(jogadorLogico);
        }
    }
    for (int i = 0; i < oponentesVisuais.size(); ++i) {
        for (int j = i + 1; j < oponentesVisuais.size(); ++j) {
            if (oponentesVisuais[i]->collidesWithItem(oponentesVisuais[j], Qt::IntersectsItemShape)) {
                oponentesLogicos[i]->colidirComBarco(oponentesLogicos[j]);
                oponentesLogicos[j]->colidirComBarco(oponentesLogicos[i]);
            }
        }
    }

    view->centerOn(jogadorVisual);

    // Oponentes
    for (int i = 0; i < oponentesLogicos.size(); ++i) {
        BarcoOponente* op = oponentesLogicos[i];
        if (!op->isTerminado()) {
            op->tomarDecisao(pista);
            op->atualizar(0.016f);

            Ponto2D posObsOp;
            if (pista->verificarColisaoVisual(oponentesVisuais[i], posObsOp)) op->chocar(posObsOp);

            const auto& checkpoints = pista->getCheckpoints();
            int idAlvoOp = op->getProximoCheckpointId();
            if (idAlvoOp < checkpoints.size()) {
                if (checkpoints[idAlvoOp]->cruzouLinha(posicoesAnterioresOponentes[i], op->getPosicao())) {
                    op->incrementarCheckpoint(checkpoints.size());
                    if (op->getVoltaAtual() > 3) {
                        op->setTerminado(true);
                        barcosFinalizados.append(op);
                    }
                }
            }
        } else {
            op->atualizar(0.016f);
        }
        oponentesVisuais[i]->atualizarPosicaoTela(op->getPosicao(), op->getAngulo());

        // --- ALTERAÇÃO: PASSA DEBUG PARA OPONENTES ---
        oponentesVisuais[i]->atualizarDebugInfo(
            op->getDebugForcaMotor(),
            op->getDebugForcaArrasto(),
            op->getDebugForcaResultante(),
            op->getDebugAnguloLeme()
            );
    }

    barraBateria->setValue((int)jogadorLogico->getBateria());
    if (jogadorLogico->getBateria() < 300) barraBateria->setStyleSheet("QProgressBar::chunk { background-color: red; }");
    else barraBateria->setStyleSheet("QProgressBar::chunk { background-color: #00ff00; }");

    int kmh = (int)(jogadorLogico->getVelocidadeAtual() / 10.0f);
    labelVelocidade->setText(QString("%1 km/h").arg(kmh));

    if (jogadorLogico->isTerminado()) labelVoltas->setText("FINALIZADO");
    else {
        int v = jogadorLogico->getVoltaAtual();
        if (v == 0) labelVoltas->setText("LARGADA");
        else labelVoltas->setText(QString("Volta: %1/3").arg(v));
    }

    atualizarRanking();

    if (barcosFinalizados.size() == todosBarcos.size()) {
        fimDeJogo(barcosFinalizados.contains(jogadorLogico));
    }
}
