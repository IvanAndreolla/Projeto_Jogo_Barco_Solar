#include "pista.h"
#include <QGraphicsPixmapItem> // Mudou de Ellipse para Pixmap
#include <QGraphicsLineItem>
#include <cstdlib>
#include <cmath>
#include <utility>
#include <QDebug>

Pista::Pista() {
    // Carregar imagens no construtor (Cache)
    imgIlha.load(":/ilha.png");
    imgBoiaVermelha.load(":/boia_vermelha.png");
    imgBoiaVerde.load(":/boia_verde.png");

    if (imgIlha.isNull()) qDebug() << "AVISO: ilha.png nao carregou!";
    if (imgBoiaVermelha.isNull()) qDebug() << "AVISO: boia_vermelha.png nao carregou!";
}

void Pista::criarIlha(QGraphicsScene *scene, float x, float y, float diametro) {
    float raio = diametro / 2.0f;

    // 1. Lógica (Colisão)
    Obstaculo obs;
    obs.posicao = Ponto2D(x, y);
    obs.raio = raio * 0.8f; // Colisão um pouco menor que o visual para ser "justo"
    listaObstaculos.append(obs);

    // 2. Visual (Sprite)
    float iso_x = x - y;
    float iso_y = (x + y) / 2.0f;

    // Se a imagem não carregou, desenha uma elipse verde (fallback)
    if (imgIlha.isNull()) {
        float alturaVisual = diametro * 0.5f;
        QGraphicsEllipseItem *elipse = scene->addEllipse(-raio, -alturaVisual/2, diametro, alturaVisual, Qt::NoPen, QBrush(Qt::green));
        elipse->setPos(iso_x, iso_y);
        elipse->setZValue(5);
        return;
    }

    // Cria o item com a imagem
    QGraphicsPixmapItem *item = scene->addPixmap(imgIlha);

    // Calcular Escala: Queremos que a largura da imagem fique igual ao 'diametro' desejado
    float escala = diametro / (float)imgIlha.width();
    item->setScale(escala);

    float offset_x = (imgIlha.width() * escala) / 2.0f;
    float offset_y = (imgIlha.height() * escala) / 2.0f;

    item->setPos(iso_x - offset_x, iso_y - offset_y - (offset_y * 0.2f));

    item->setZValue(5);
}

void Pista::criarBoia(QGraphicsScene *scene, float x, float y, QColor cor) {
    float diametro = 160.0f;
    float raio = diametro / 2.0f;

    // Lógica
    Obstaculo obs;
    obs.posicao = Ponto2D(x, y);
    obs.raio = raio;
    listaObstaculos.append(obs);

    // Visual
    float iso_x = x - y;
    float iso_y = (x + y) / 2.0f;

    // Escolhe a imagem baseada na cor
    QPixmap* refImg = (cor == Qt::red) ? &imgBoiaVermelha : &imgBoiaVerde;

    if (refImg->isNull()) {
        scene->addEllipse(iso_x - raio, iso_y - raio/2, diametro, diametro*0.5, Qt::NoPen, QBrush(cor));
        return;
    }

    QGraphicsPixmapItem *item = scene->addPixmap(*refImg);

    float escala = diametro / (float)refImg->width();
    item->setScale(escala);

    float offset_x = (refImg->width() * escala) / 2.0f;
    float offset_y = (refImg->height() * escala) / 2.0f;

    item->setPos(iso_x - offset_x, iso_y - offset_y - (offset_y * 0.3f));
    item->setZValue(6);
}

void Pista::gerarCenario(QGraphicsScene *scene) {
    listaObstaculos.clear();
    listaCheckpoints.clear();

    // 1. Ilhas
    criarIlha(scene, 2500, 2500, 1200); // Central
    criarIlha(scene, 500, 500, 1000);
    criarIlha(scene, 4500, 500, 1000);
    criarIlha(scene, 500, 4500, 1000);
    criarIlha(scene, 4500, 4500, 1000);

    // 2. Circuito
    Ponto2D centroMapa(2500, 2500);
    float raioInterno = 1000.0f;
    float raioExterno = 1800.0f;
    int qtdSetores = 16;

    for (int i = 0; i < qtdSetores; ++i) {
        float angulo = (i * 2 * 3.14159f) / qtdSetores;

        float inX = centroMapa.x + std::cos(angulo) * raioInterno;
        float inY = centroMapa.y + std::sin(angulo) * raioInterno;

        float outX = centroMapa.x + std::cos(angulo) * raioExterno;
        float outY = centroMapa.y + std::sin(angulo) * raioExterno;

        criarBoia(scene, inX, inY, Qt::red);
        criarBoia(scene, outX, outY, Qt::green);

        Checkpoint* cp = new Checkpoint(i, Ponto2D(inX, inY), Ponto2D(outX, outY));
        listaCheckpoints.append(cp);

        // Linha de Largada (Branca)
        if (i == 0) {
            float isoInX = inX - inY; float isoInY = (inX + inY)/2.0f;
            float isoOutX = outX - outY; float isoOutY = (outX + outY)/2.0f;
            QGraphicsLineItem* linha = scene->addLine(isoInX, isoInY, isoOutX, isoOutY, QPen(Qt::white, 8));
            linha->setZValue(2);
        }
    }
}

bool Pista::verificarColisao(BarcoSolar *barco) {
    for (const Obstaculo &obs : std::as_const(listaObstaculos)) {
        if (barco->getPosicao().dist(obs.posicao) < (obs.raio + 15.0f)) {
            return true;
        }
    }
    return false;
}

const QList<Checkpoint*>& Pista::getCheckpoints() const {
    return listaCheckpoints;
}
