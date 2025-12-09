#include "pista.h"
#include <QGraphicsPixmapItem>
#include <QGraphicsLineItem>
#include <QGraphicsEllipseItem>
#include <cstdlib>
#include <cmath>
#include <QDebug>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Pista::Pista() {
    imgIlha.load(":/ilha.png");
    imgBoiaVermelha.load(":/boia_vermelha.png");
    imgBoiaVerde.load(":/boia_verde.png");
    imgPonte.load(":/ponte.png");

    if (imgIlha.isNull()) qDebug() << "AVISO: ilha.png nao carregou";
    if (imgPonte.isNull()) qDebug() << "AVISO: ponte.png nao carregou (adicione ao resource)";

    posicaoLargada = Ponto2D(2500, 2500);
}

void Pista::criarIlha(QGraphicsScene *scene, float x, float y, float diametro) {
    float raio = diametro / 2.0f;
    float raioLogico = diametro * 0.4f;

    Obstaculo obs;
    obs.posicao = Ponto2D(x, y);
    obs.raio = raioLogico;
    listaObstaculos.append(obs);

    float iso_x = x - y;
    float iso_y = (x + y) / 2.0f;

    if (!imgIlha.isNull()) {
        QGraphicsPixmapItem *item = scene->addPixmap(imgIlha);
        item->setShapeMode(QGraphicsPixmapItem::HeuristicMaskShape);

        float escala = diametro / (float)imgIlha.width();
        item->setScale(escala);

        float w = imgIlha.width() * escala;
        float h = imgIlha.height() * escala;

        item->setPos(iso_x - w/2, iso_y - h/2 - (h * 0.2f));
        item->setZValue(iso_y); // Z-Value baseado na posição Y (profundidade normal)

        obstaculosVisuais.append(item);
    } else {
        float hVis = diametro * 0.5f;
        QGraphicsEllipseItem* el = scene->addEllipse(iso_x - raio, iso_y - hVis/2, diametro, hVis, Qt::NoPen, QBrush(Qt::green));
        el->setZValue(5);
        obstaculosVisuais.append(el);
    }
}

void Pista::criarBoia(QGraphicsScene *scene, float x, float y, QColor cor) {
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

        item->setPos(iso_x - w/2, iso_y - h/2 - (h * 0.3f));
        item->setZValue(iso_y);

        obstaculosVisuais.append(item);
    } else {
        float hVis = diametro * 0.5f;
        QGraphicsEllipseItem* el = scene->addEllipse(iso_x - raio, iso_y - hVis/2, diametro, hVis, Qt::NoPen, QBrush(cor));
        el->setZValue(5);
        obstaculosVisuais.append(el);
    }
}

void Pista::gerarCenario(QGraphicsScene *scene, Tipo tipo) {
    obstaculosVisuais.clear();
    listaObstaculos.clear();
    listaCheckpoints.clear();

    auto iso = [](float x, float y) { return QPointF(x - y, (x + y) / 2.0f); };

    // ====================================================================
    // PISTA 1: OVAL
    // ====================================================================
    if (tipo == OVAL_FACIL) {
        criarIlha(scene, 2500, 2500, 1200);
        criarIlha(scene, 500, 500, 1000);   criarIlha(scene, 4500, 500, 1000);
        criarIlha(scene, 500, 4500, 1000);  criarIlha(scene, 4500, 4500, 1000);

        Ponto2D centro(2500, 2500);
        float rIn = 1000.0f; float rOut = 1800.0f;

        for (int i = 0; i < 16; ++i) {
            float ang = (i * 2 * M_PI) / 16;
            float inX = centro.x + std::cos(ang) * rIn; float inY = centro.y + std::sin(ang) * rIn;
            float outX = centro.x + std::cos(ang) * rOut; float outY = centro.y + std::sin(ang) * rOut;

            criarBoia(scene, inX, inY, Qt::red);
            criarBoia(scene, outX, outY, Qt::green);
            listaCheckpoints.append(new Checkpoint(i, Ponto2D(inX, inY), Ponto2D(outX, outY)));

            if (i == 0) {
                QGraphicsLineItem* linha = scene->addLine(iso(inX, inY).x(), iso(inX, inY).y(), iso(outX, outY).x(), iso(outX, outY).y(), QPen(Qt::white, 10));
                linha->setZValue(0);
                float angStart = (15.0f * 2 * M_PI) / 16;
                posicaoLargada = Ponto2D(centro.x + std::cos(angStart) * 1400, centro.y + std::sin(angStart) * 1400);
            }
        }
    }
    // ====================================================================
    // PISTA 2: OITO (Com Ponte Hercílio Luz)
    // ====================================================================
    else if (tipo == OITO_MEDIO) {
        Ponto2D c1(1500, 2500);
        Ponto2D c2(3500, 2500);

        criarIlha(scene, 1500, 2500, 600);
        criarIlha(scene, 3500, 2500, 600);

        // --- ADICIONANDO A PONTE HERCÍLIO LUZ ---
        if (!imgPonte.isNull()) {
            QGraphicsPixmapItem* itemPonte = scene->addPixmap(imgPonte);
            itemPonte->setTransformationMode(Qt::SmoothTransformation);

            float escalaPonte = 3.5f;
            itemPonte->setScale(escalaPonte);

            // Centraliza no cruzamento (2500, 2500)
            float w = imgPonte.width() * escalaPonte;
            float h = imgPonte.height() * escalaPonte;
            QPointF centroIso = iso(2500, 2500);

            itemPonte->setPos(centroIso.x() - w/2, centroIso.y() - h/2);

            // Z-Value MÁGICO:
            // Nuvens = 20000
            // Barcos = ~iso_y (varia de 0 a 5000)
            // Ponte = 10000 (Bem acima dos barcos, mas abaixo das nuvens)
            itemPonte->setZValue(10000);

        }

        int idCP = 0;

        for (int i = 1; i <= 6; ++i) {
            float ang = (i * 2 * M_PI) / 8;
            float inX = c1.x + std::cos(ang)*500; float inY = c1.y + std::sin(ang)*500;
            float outX = c1.x + std::cos(ang)*1200; float outY = c1.y + std::sin(ang)*1200;

            criarBoia(scene, inX, inY, Qt::red); criarBoia(scene, outX, outY, Qt::green);
            listaCheckpoints.append(new Checkpoint(idCP++, Ponto2D(inX, inY), Ponto2D(outX, outY)));
        }

        listaCheckpoints.append(new Checkpoint(idCP++, Ponto2D(2500, 2000), Ponto2D(2500, 3000)));

        int indices[] = {3, 2, 1, 0, 7, 6};

        for (int k = 0; k < 6; ++k) {
            int i = indices[k];
            float ang = (i * 2 * M_PI) / 8;

            float inX = c2.x + std::cos(ang)*500; float inY = c2.y + std::sin(ang)*500;
            float outX = c2.x + std::cos(ang)*1200; float outY = c2.y + std::sin(ang)*1200;

            criarBoia(scene, inX, inY, Qt::green); criarBoia(scene, outX, outY, Qt::red);
            listaCheckpoints.append(new Checkpoint(idCP++, Ponto2D(inX, inY), Ponto2D(outX, outY)));
        }

        posicaoLargada = Ponto2D(2000, 2000);
        if(!listaCheckpoints.isEmpty()) {
            Checkpoint* cp0 = listaCheckpoints[0];
            scene->addLine(iso(cp0->getP1().x, cp0->getP1().y).x(), iso(cp0->getP1().x, cp0->getP1().y).y(), iso(cp0->getP2().x, cp0->getP2().y).x(), iso(cp0->getP2().x, cp0->getP2().y).y(), QPen(Qt::white, 10))->setZValue(0);
        }
    }
    // ====================================================================
    // PISTA 3: SINUOSA ESTRATÉGICA
    // ====================================================================
    else {
        Ponto2D centro(2500, 2500);
        int numOndas = 5;

        criarIlha(scene, 2500, 2500, 800);

        for(int k = 0; k < numOndas; ++k) {
            float angBase = k * (2 * M_PI / numOndas);
            float angPico = angBase + (M_PI / (numOndas * 2));

            float xIn = centro.x + std::cos(angPico) * 1000.0f;
            float yIn = centro.y + std::sin(angPico) * 1000.0f;
            criarIlha(scene, xIn, yIn, 400);

            float angVale = angBase - (M_PI / (numOndas * 2));
            float xOut = centro.x + std::cos(angVale) * 2200.0f;
            float yOut = centro.y + std::sin(angVale) * 2200.0f;
            criarIlha(scene, xOut, yOut, 500);
        }

        criarIlha(scene, 200, 200, 600);
        criarIlha(scene, 4800, 4800, 600);

        int numPassos = 60;
        float raioBase = 1500.0f;
        float amplitudeSinuosidade = 350.0f;
        float larguraPista = 600.0f;

        for (int i = 0; i < numPassos; ++i) {
            float t = (float)i / numPassos;
            float ang = t * 2 * M_PI;
            float rAtual = raioBase + amplitudeSinuosidade * std::sin(ang * numOndas);

            float cx = centro.x + std::cos(ang) * rAtual;
            float cy = centro.y + std::sin(ang) * rAtual;

            float cxNext = centro.x + std::cos(ang + 0.01) * (raioBase + amplitudeSinuosidade * std::sin((ang + 0.01) * numOndas));
            float cyNext = centro.y + std::sin(ang + 0.01) * (raioBase + amplitudeSinuosidade * std::sin((ang + 0.01) * numOndas));

            Ponto2D dir(cxNext - cx, cyNext - cy);
            dir.normalize();
            Ponto2D perp(-dir.y, dir.x);

            Ponto2D pInterno = Ponto2D(cx, cy) + (perp * (larguraPista / 2.0f));
            Ponto2D pExterno = Ponto2D(cx, cy) - (perp * (larguraPista / 2.0f));

            bool checkpointVisivel = (i % 3 == 0);

            if (checkpointVisivel) {
                criarBoia(scene, pInterno.x, pInterno.y, Qt::red);
                criarBoia(scene, pExterno.x, pExterno.y, Qt::green);
            }
            else {
                float sz = 25.0f;
                QPointF posIso = iso(cx, cy);
                auto marcador = scene->addEllipse(posIso.x() - sz/2, posIso.y() - sz/2, sz, sz, Qt::NoPen, QBrush(QColor(255, 255, 0, 150)));
                marcador->setZValue(0);
            }

            listaCheckpoints.append(new Checkpoint(i, pInterno, pExterno));

            if (i == 0) {
                scene->addLine(iso(pInterno.x, pInterno.y).x(), iso(pInterno.x, pInterno.y).y(), iso(pExterno.x, pExterno.y).x(), iso(pExterno.x, pExterno.y).y(), QPen(Qt::white, 10))->setZValue(0);

                float angStart = ((numPassos - 2.0f) / numPassos) * 2 * M_PI;
                float rStart = raioBase + amplitudeSinuosidade * std::sin(angStart * numOndas);
                posicaoLargada = Ponto2D(centro.x + std::cos(angStart) * rStart, centro.y + std::sin(angStart) * rStart);
            }
        }
    }
}

bool Pista::verificarColisaoVisual(QGraphicsItem *itemBarco, Ponto2D &posObstaculo) {
    for (int i = 0; i < obstaculosVisuais.size(); ++i) {
        if (obstaculosVisuais[i] && itemBarco->collidesWithItem(obstaculosVisuais[i])) {
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
