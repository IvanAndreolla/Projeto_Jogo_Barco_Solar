#include "gamemanager.h"
#include <cstdlib>
#include <QDebug>
#include <QFont>

GameManager::GameManager(QGraphicsScene *scene, QGraphicsView *view,
                         QProgressBar *bar, QLabel *labelVel, QLabel *labelVoltas,
                         QObject *parent)
    : QObject(parent), scene(scene), view(view),
    barraBateria(bar), labelVelocidade(labelVel), labelVoltas(labelVoltas)
{
    inputManager = new InputManager();
    pista = new Pista();
    pista->gerarCenario(scene);

    // Posiciona ANTES da linha de largada
    float cx = 2500.0f;
    float cy = 2500.0f;
    float raioRaia = 1400.0f;
    float anguloStart = 350.0f * (3.14159f / 180.0f);
    float startX = cx + std::cos(anguloStart) * raioRaia;
    float startY = cy + std::sin(anguloStart) * raioRaia;

    jogadorLogico = new BarcoJogador(startX, startY);
    jogadorVisual = new BarcoGrafico();
    scene->addItem(jogadorVisual);

    posicaoAnteriorJogador = jogadorLogico->getPosicao();

    for (int i = 0; i < 3; ++i) {
        float anguloOp = (340.0f - (i * 10.0f)) * (3.14159f / 180.0f);
        float opX = cx + std::cos(anguloOp) * raioRaia;
        float opY = cy + std::sin(anguloOp) * raioRaia;

        BarcoOponente* oponente = new BarcoOponente(opX, opY);
        BarcoGrafico* visual = new BarcoGrafico();
        scene->addItem(visual);

        oponentesLogicos.append(oponente);
        oponentesVisuais.append(visual);
    }

    jogoAtivo = true;
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

void GameManager::fimDeJogo(bool venceu) {
    jogoAtivo = false;
    timer->stop();

    QString texto = venceu ? "VITÓRIA!\nCorrida Completada!" : "BATERIA ESGOTADA\nGame Over";
    QColor cor = venceu ? Qt::green : Qt::red;

    QGraphicsTextItem* mensagem = scene->addText(texto);

    QFont fonte("Arial", 36, QFont::Bold);
    mensagem->setFont(fonte);
    mensagem->setDefaultTextColor(cor);

    Ponto2D pos = jogadorLogico->getPosicao();
    float iso_x = pos.x - pos.y;
    float iso_y = (pos.x + pos.y) / 2.0f;

    mensagem->setPos(iso_x - 200, iso_y - 100);
    mensagem->setZValue(100);
}

void GameManager::gameLoop() {
    if (!jogoAtivo) return;

    posicaoAnteriorJogador = jogadorLogico->getPosicao();

    // --- JOGADOR ---
    jogadorLogico->processarInput(inputManager);
    jogadorLogico->atualizar(0.016f);

    if (jogadorLogico->getBateria() <= 0) {
        fimDeJogo(false);
        return;
    }

    if (pista->verificarColisao(jogadorLogico)) {
        jogadorLogico->chocar();
    }

    const auto& checkpoints = pista->getCheckpoints();
    int idAlvo = jogadorLogico->getProximoCheckpointId();

    if (idAlvo < checkpoints.size()) {
        Checkpoint* cp = checkpoints[idAlvo];

        // Jogador: Usa LINHA (precisão e anti-trapaça)
        if (cp->cruzouLinha(posicaoAnteriorJogador, jogadorLogico->getPosicao())) {

            jogadorLogico->incrementarCheckpoint(checkpoints.size());

            if (idAlvo == 0) {
                labelVelocidade->setText("VOLTA COMPLETADA!");
            }

            if (jogadorLogico->getVoltaAtual() > 3) {
                fimDeJogo(true);
                return;
            }
        }
    }

    jogadorVisual->atualizarPosicaoTela(jogadorLogico->getPosicao(), jogadorLogico->getAngulo());
    view->centerOn(jogadorVisual);

    // --- OPONENTES ---
    for (int i = 0; i < oponentesLogicos.size(); ++i) {
        BarcoOponente* opLogico = oponentesLogicos[i];
        BarcoGrafico* opVisual = oponentesVisuais[i];

        opLogico->tomarDecisao(pista);
        opLogico->atualizar(0.016f);

        if (pista->verificarColisao(opLogico)) {
            opLogico->chocar();
        }

        int idAlvoOp = opLogico->getProximoCheckpointId();
        if (idAlvoOp < checkpoints.size()) {
            float distParaMeta = opLogico->getPosicao().dist(checkpoints[idAlvoOp]->getCentro());

            if (distParaMeta < 400.0f) {
                opLogico->incrementarCheckpoint(checkpoints.size());
            }
        }

        opVisual->atualizarPosicaoTela(opLogico->getPosicao(), opLogico->getAngulo());
    }

    // --- HUD ---
    barraBateria->setValue((int)jogadorLogico->getBateria());

    if (jogadorLogico->getBateria() < 300)
        barraBateria->setStyleSheet("QProgressBar::chunk { background-color: red; }");
    else
        barraBateria->setStyleSheet("QProgressBar::chunk { background-color: #00ff00; }");

    if (labelVelocidade->text() != "VOLTA COMPLETADA!") {
        int kmh = (int)(jogadorLogico->getVelocidadeAtual() / 10.0f);
        labelVelocidade->setText(QString("%1 km/h").arg(kmh));
    } else {
        if (rand() % 50 == 0) labelVelocidade->setText("");
    }

    int v = jogadorLogico->getVoltaAtual();

    if (v == 0)
        labelVoltas->setText("Posicione para a LARGADA");
    else if (v <= 3)
        labelVoltas->setText(QString("Volta: %1/3").arg(v));
    else
        labelVoltas->setText("FINALIZADO");
}
