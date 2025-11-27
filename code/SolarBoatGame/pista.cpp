#include "pista.h"
#include <QGraphicsPixmapItem>
#include <QGraphicsLineItem>
#include <QGraphicsEllipseItem>
#include <cstdlib>
#include <cmath>
#include <utility>
#include <QDebug>

Pista::Pista() {
    imgIlha.load(":/ilha.png");
    imgBoiaVermelha.load(":/boia_vermelha.png");
    imgBoiaVerde.load(":/boia_verde.png");

    if (imgIlha.isNull()) qDebug() << "AVISO: ilha.png nao carregou";
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

        // Centralização no pé da ilha
        item->setPos(iso_x - w/2, iso_y - h/2 - (h * 0.2f));

        // Z inicial (será corrigido pelo GameManager a cada frame)
        item->setZValue(iso_y);

        obstaculosVisuais.append(item);
    } else {
        float hVis = diametro * 0.5f;
        QGraphicsEllipseItem* el = scene->addEllipse(iso_x - raio, iso_y - hVis/2, diametro, hVis, Qt::NoPen, QBrush(Qt::green));
        el->setZValue(5);
        obstaculosVisuais.append(el);
    }
}

void Pista::criarBoia(QGraphicsScene *scene, float x, float y, QColor cor) {
    // Antes de criar, checa se já tem algo neste local (x,y)
    for (const Obstaculo& obsExistente : listaObstaculos) {
        // Se a distância for menor que a soma dos raios + uma margem de 100px
        if (Ponto2D(x, y).dist(obsExistente.posicao) < (obsExistente.raio + 100.0f)) {
            return; // Cancela a criação desta boia (ela cairia em cima de algo)
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

    if (tipo == OVAL_FACIL) {
        criarIlha(scene, 2500, 2500, 1200);
        criarIlha(scene, 500, 500, 1000);   criarIlha(scene, 4500, 500, 1000);
        criarIlha(scene, 500, 4500, 1000);  criarIlha(scene, 4500, 4500, 1000);

        Ponto2D centro(2500, 2500);
        float rIn = 1000.0f; float rOut = 1800.0f;

        for (int i = 0; i < 16; ++i) {
            float ang = (i * 2 * 3.14159f) / 16;
            float inX = centro.x + std::cos(ang) * rIn; float inY = centro.y + std::sin(ang) * rIn;
            float outX = centro.x + std::cos(ang) * rOut; float outY = centro.y + std::sin(ang) * rOut;

            criarBoia(scene, inX, inY, Qt::red);
            criarBoia(scene, outX, outY, Qt::green);

            listaCheckpoints.append(new Checkpoint(i, Ponto2D(inX, inY), Ponto2D(outX, outY)));

            if (i == 0) {
                QGraphicsLineItem* linha = scene->addLine(iso(inX, inY).x(), iso(inX, inY).y(), iso(outX, outY).x(), iso(outX, outY).y(), QPen(Qt::white, 10));
                linha->setZValue(0);
                float angStart = (15.0f * 2 * 3.14159f) / 16;
                posicaoLargada = Ponto2D(centro.x + std::cos(angStart) * 1400, centro.y + std::sin(angStart) * 1400);
            }
        }
    }
    else if (tipo == OITO_MEDIO) {
        Ponto2D c1(1500, 2500); Ponto2D c2(3500, 2500);
        criarIlha(scene, 1500, 2500, 600); criarIlha(scene, 3500, 2500, 600);

        int idCP = 0;
        for (int i = 0; i < 8; ++i) {
            if (i == 0 || i == 7) continue;
            float ang = (i * 2 * 3.14159f) / 8;
            float inX = c1.x + std::cos(ang)*500; float inY = c1.y + std::sin(ang)*500;
            float outX = c1.x + std::cos(ang)*900; float outY = c1.y + std::sin(ang)*900;
            criarBoia(scene, inX, inY, Qt::red); criarBoia(scene, outX, outY, Qt::green);
            listaCheckpoints.append(new Checkpoint(idCP++, Ponto2D(inX, inY), Ponto2D(outX, outY)));
        }
        listaCheckpoints.append(new Checkpoint(idCP++, Ponto2D(2500, 2000), Ponto2D(2500, 3000)));
        for (int i = 4; i < 12; ++i) {
            if (i == 8 || i == 12) continue;
            float ang = 3.14159f + (i * 2 * 3.14159f) / 8;
            float inX = c2.x + std::cos(ang)*500; float inY = c2.y + std::sin(ang)*500;
            float outX = c2.x + std::cos(ang)*900; float outY = c2.y + std::sin(ang)*900;
            criarBoia(scene, inX, inY, Qt::green); criarBoia(scene, outX, outY, Qt::red);
            listaCheckpoints.append(new Checkpoint(idCP++, Ponto2D(outX, outY), Ponto2D(inX, inY)));
        }
        posicaoLargada = Ponto2D(1500, 1500);
        Checkpoint* cp0 = listaCheckpoints[0];
        scene->addLine(iso(cp0->getP1().x, cp0->getP1().y).x(), iso(cp0->getP1().x, cp0->getP1().y).y(), iso(cp0->getP2().x, cp0->getP2().y).x(), iso(cp0->getP2().x, cp0->getP2().y).y(), QPen(Qt::white, 10))->setZValue(0);
    }
    else {
        for(int x=500; x<4500; x+=600) { criarIlha(scene, x, 800, 400); criarIlha(scene, x, 4200, 400); }
        int idCP = 0;
        for(int i=0; i<10; ++i) {
            float x = 800 + i * 450;
            float y = 2500 + ((i%2==0) ? -600 : 600);
            if(x < 4000) criarIlha(scene, x + 225, 2500, 300);
            criarBoia(scene, x, y-200, Qt::red); criarBoia(scene, x, y+200, Qt::green);
            listaCheckpoints.append(new Checkpoint(idCP++, Ponto2D(x, y-200), Ponto2D(x, y+200)));
            if(i==0) {
                posicaoLargada = Ponto2D(x-300, y);
                scene->addLine(iso(x, y-200).x(), iso(x, y-200).y(), iso(x, y+200).x(), iso(x, y+200).y(), QPen(Qt::white, 10))->setZValue(0);
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
