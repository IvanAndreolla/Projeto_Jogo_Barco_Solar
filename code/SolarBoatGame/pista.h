#ifndef PISTA_H
#define PISTA_H

#include <QList>
#include <QGraphicsScene>
#include <QPixmap>
#include "ponto2d.h"
#include "barcosolar.h"
#include "checkpoint.h"

class Pista
{
public:
    Pista();

    void gerarCenario(QGraphicsScene *scene);
    bool verificarColisao(BarcoSolar *barco);
    const QList<Checkpoint*>& getCheckpoints() const;

private:
    struct Obstaculo {
        Ponto2D posicao;
        float raio;
    };
    QList<Obstaculo> listaObstaculos;
    QList<Checkpoint*> listaCheckpoints;

    // Cache de Imagens
    QPixmap imgIlha;
    QPixmap imgBoiaVermelha;
    QPixmap imgBoiaVerde;

    void criarIlha(QGraphicsScene *scene, float x, float y, float diametro);
    void criarBoia(QGraphicsScene *scene, float x, float y, QColor cor);
};

#endif // PISTA_H
