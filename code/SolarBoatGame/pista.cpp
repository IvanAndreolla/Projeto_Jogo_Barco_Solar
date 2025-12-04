#include "pista.h"
#include <QGraphicsPixmapItem>
#include <QGraphicsLineItem>
#include <QGraphicsEllipseItem>
#include <cstdlib>
#include <cmath>
#include <QDebug>

// Define PI caso a biblioteca math.h não tenha definido
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Pista::Pista() {
    // Carrega os recursos de imagem na memória
    imgIlha.load(":/ilha.png");
    imgBoiaVermelha.load(":/boia_vermelha.png");
    imgBoiaVerde.load(":/boia_verde.png");

    if (imgIlha.isNull()) qDebug() << "AVISO: ilha.png nao carregou";

    // Define uma posição de largada padrão
    posicaoLargada = Ponto2D(2500, 2500);
}

void Pista::criarIlha(QGraphicsScene *scene, float x, float y, float diametro) {
    float raio = diametro / 2.0f;
    // Define o raio lógico de colisão menor que o visual (tolerância)
    float raioLogico = diametro * 0.4f;

    Obstaculo obs;
    obs.posicao = Ponto2D(x, y);
    obs.raio = raioLogico;
    listaObstaculos.append(obs);

    // Converte coordenadas cartesianas para isométricas de tela
    float iso_x = x - y;
    float iso_y = (x + y) / 2.0f;

    if (!imgIlha.isNull()) {
        QGraphicsPixmapItem *item = scene->addPixmap(imgIlha);
        // Ajusta a máscara de colisão para o contorno da imagem
        item->setShapeMode(QGraphicsPixmapItem::HeuristicMaskShape);

        float escala = diametro / (float)imgIlha.width();
        item->setScale(escala);

        float w = imgIlha.width() * escala;
        float h = imgIlha.height() * escala;

        // Centraliza o sprite e ajusta a ordem de renderização (Z-index)
        item->setPos(iso_x - w/2, iso_y - h/2 - (h * 0.2f));
        item->setZValue(iso_y);

        obstaculosVisuais.append(item);
    } else {
        // Renderiza uma elipse verde caso a imagem falhe
        float hVis = diametro * 0.5f;
        QGraphicsEllipseItem* el = scene->addEllipse(iso_x - raio, iso_y - hVis/2, diametro, hVis, Qt::NoPen, QBrush(Qt::green));
        el->setZValue(5);
        obstaculosVisuais.append(el);
    }
}

void Pista::criarBoia(QGraphicsScene *scene, float x, float y, QColor cor) {
    // Verifica sobreposição com obstáculos existentes para evitar colisão na criação
    for (int i = 0; i < listaObstaculos.size(); ++i) {
        if (Ponto2D(x, y).dist(listaObstaculos[i].posicao) < (listaObstaculos[i].raio + 100.0f)) {
            return;
        }
    }

    float diametro = 120.0f;
    float raio = diametro / 2.0f;
    float raioLogico = diametro * 0.3f;

    Obstaculo obs;
    obs.posicao = Ponto2D(x, y);
    obs.raio = raioLogico;
    listaObstaculos.append(obs);

    // Cálculo de projeção isométrica
    float iso_x = x - y;
    float iso_y = (x + y) / 2.0f;

    QPixmap* refImg = (cor == Qt::red) ? &imgBoiaVermelha : &imgBoiaVerde;

    if (!refImg->isNull()) {
        QGraphicsPixmapItem *item = scene->addPixmap(*refImg);
        item->setShapeMode(QGraphicsPixmapItem::HeuristicMaskShape);

        float escala = diametro / (float)refImg->width();
        item->setScale(escala);
        float w = refImg->width() * escala;
        float h = refImg->height() * escala;

        // Posiciona a boia ajustando o centro visual
        item->setPos(iso_x - w/2, iso_y - h/2 - (h * 0.3f));
        item->setZValue(iso_y);

        obstaculosVisuais.append(item);
    } else {
        // Fallback visual simples se a imagem não carregar
        float hVis = diametro * 0.5f;
        QGraphicsEllipseItem* el = scene->addEllipse(iso_x - raio, iso_y - hVis/2, diametro, hVis, Qt::NoPen, QBrush(cor));
        el->setZValue(5);
        obstaculosVisuais.append(el);
    }
}

void Pista::gerarCenario(QGraphicsScene *scene, Tipo tipo) {
    // Reseta listas de obstáculos e checkpoints
    obstaculosVisuais.clear();
    listaObstaculos.clear();
    listaCheckpoints.clear();

    // Lambda para conversão rápida Cartesiano -> Isométrico
    auto iso = [](float x, float y) { return QPointF(x - y, (x + y) / 2.0f); };

    // ====================================================================
    // PISTA 1: OVAL
    // ====================================================================
    if (tipo == OVAL_FACIL) {
        // Cria ilhas nas bordas e centro
        criarIlha(scene, 2500, 2500, 1200);
        criarIlha(scene, 500, 500, 1000);   criarIlha(scene, 4500, 500, 1000);
        criarIlha(scene, 500, 4500, 1000);  criarIlha(scene, 4500, 4500, 1000);

        Ponto2D centro(2500, 2500);
        float rIn = 1000.0f; float rOut = 1800.0f;

        // Gera boias e checkpoints em disposição circular
        for (int i = 0; i < 16; ++i) {
            float ang = (i * 2 * 3.14159f) / 16;
            float inX = centro.x + std::cos(ang) * rIn; float inY = centro.y + std::sin(ang) * rIn;
            float outX = centro.x + std::cos(ang) * rOut; float outY = centro.y + std::sin(ang) * rOut;

            criarBoia(scene, inX, inY, Qt::red);
            criarBoia(scene, outX, outY, Qt::green);

            listaCheckpoints.append(new Checkpoint(i, Ponto2D(inX, inY), Ponto2D(outX, outY)));

            // Desenha a linha de largada no índice 0
            if (i == 0) {
                QGraphicsLineItem* linha = scene->addLine(iso(inX, inY).x(), iso(inX, inY).y(), iso(outX, outY).x(), iso(outX, outY).y(), QPen(Qt::white, 10));
                linha->setZValue(0);
                float angStart = (15.0f * 2 * 3.14159f) / 16;
                posicaoLargada = Ponto2D(centro.x + std::cos(angStart) * 1400, centro.y + std::sin(angStart) * 1400);
            }
        }
    }
    // ====================================================================
    // PISTA 2: OITO
    // ====================================================================
    else if (tipo == OITO_MEDIO) {
        Ponto2D c1(1500, 2500); // Centro Esquerda
        Ponto2D c2(3500, 2500); // Centro Direita

        criarIlha(scene, 1500, 2500, 600);
        criarIlha(scene, 3500, 2500, 600);

        int idCP = 0;

        // Gera o primeiro loop (Esquerda)
        for (int i = 1; i <= 6; ++i) {
            float ang = (i * 2 * 3.14159f) / 8;
            float inX = c1.x + std::cos(ang)*500; float inY = c1.y + std::sin(ang)*500;
            float outX = c1.x + std::cos(ang)*1200; float outY = c1.y + std::sin(ang)*1200;

            criarBoia(scene, inX, inY, Qt::red); criarBoia(scene, outX, outY, Qt::green);
            listaCheckpoints.append(new Checkpoint(idCP++, Ponto2D(inX, inY), Ponto2D(outX, outY)));
        }

        // Adiciona checkpoint de transição
        listaCheckpoints.append(new Checkpoint(idCP++, Ponto2D(2500, 2000), Ponto2D(2500, 3000)));

        int indices[] = {3, 2, 1, 0, 7, 6};

        // Gera o segundo loop (Direita) com ordem invertida para cruzar o 8
        for (int k = 0; k < 6; ++k) {
            int i = indices[k];
            float ang = (i * 2 * 3.14159f) / 8;

            float inX = c2.x + std::cos(ang)*500; float inY = c2.y + std::sin(ang)*500;
            float outX = c2.x + std::cos(ang)*1200; float outY = c2.y + std::sin(ang)*1200;

            criarBoia(scene, inX, inY, Qt::green); criarBoia(scene, outX, outY, Qt::red);
            listaCheckpoints.append(new Checkpoint(idCP++, Ponto2D(inX, inY), Ponto2D(outX, outY)));
        }

        // Define largada e linha visual
        posicaoLargada = Ponto2D(2000, 2000);
        if(!listaCheckpoints.isEmpty()) {
            Checkpoint* cp0 = listaCheckpoints[0];
            scene->addLine(iso(cp0->getP1().x, cp0->getP1().y).x(), iso(cp0->getP1().x, cp0->getP1().y).y(), iso(cp0->getP2().x, cp0->getP2().y).x(), iso(cp0->getP2().x, cp0->getP2().y).y(), QPen(Qt::white, 10))->setZValue(0);
        }
    }
    // ====================================================================
    // PISTA 3: SLALOM
    // ====================================================================
    else {
        // Gera paredes externas do canal
        for(int x=500; x<4500; x+=600) {
            criarIlha(scene, x, 800, 400);
            criarIlha(scene, x, 4200, 400);
        }

        int idCP = 0;

        for(int i=0; i<10; ++i) {
            float x = 800 + i * 450;
            // Alterna a posição Y para criar o zigue-zague
            float y = 2500 + ((i%2==0) ? -500 : 500);

            // Largura do portão aumentada para facilitar a passagem
            float larguraGate = 200.0f;

            if (i % 2 == 0) {
                // Parte superior: Verde embaixo, Vermelha em cima
                criarBoia(scene, x, y - larguraGate, Qt::green);
                criarBoia(scene, x, y + larguraGate, Qt::red);

                listaCheckpoints.append(new Checkpoint(idCP++, Ponto2D(x, y + larguraGate), Ponto2D(x, y - larguraGate)));
            }
            else {

                criarBoia(scene, x, y - larguraGate, Qt::red);
                criarBoia(scene, x, y + larguraGate, Qt::green);

                listaCheckpoints.append(new Checkpoint(idCP++, Ponto2D(x, y - larguraGate), Ponto2D(x, y + larguraGate)));
            }

            if(i==0) {
                posicaoLargada = Ponto2D(x-300, y);
                scene->addLine(iso(x, y-larguraGate).x(), iso(x, y-larguraGate).y(), iso(x, y+larguraGate).x(), iso(x, y+larguraGate).y(), QPen(Qt::white, 10))->setZValue(0);
            }
        }
    }
}

bool Pista::verificarColisaoVisual(QGraphicsItem *itemBarco, Ponto2D &posObstaculo) {
    // Itera sobre itens visuais para checar colisão via Qt Graphics View
    for (int i = 0; i < obstaculosVisuais.size(); ++i) {
        if (obstaculosVisuais[i] && itemBarco->collidesWithItem(obstaculosVisuais[i])) {
            // Mapeia o índice visual de volta para a lista lógica de obstáculos
            if (i < listaObstaculos.size()) {
                posObstaculo = listaObstaculos[i].posicao;
                return true;
            }
        }
    }
    return false;
}

const QList<Checkpoint*>& Pista::getCheckpoints() const { return listaCheckpoints; }
const QList<QGraphicsItem*>& Pista::getObstaculosVisuais() const { return obstaculosVisuais; }
Ponto2D Pista::getPosicaoLargada() const { return posicaoLargada; }
