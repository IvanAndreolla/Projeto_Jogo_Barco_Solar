#ifndef PISTA_H
#define PISTA_H

#include <QList>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QPixmap>
#include "ponto2d.h"
#include "barcosolar.h"
#include "checkpoint.h"

class Pista
{
public:
    Pista();

    enum Tipo { OVAL_FACIL, OITO_MEDIO, SLALOM_DIFICIL };

    // Struct para lógica
    struct Obstaculo {
        Ponto2D posicao;
        float raio;
    };

    void gerarCenario(QGraphicsScene *scene, Tipo tipo);

    // Retorna a posição do obstáculo colidido para o empurrão (unstick)
    bool verificarColisaoVisual(QGraphicsItem *itemBarco, Ponto2D &posObstaculo);

    const QList<Checkpoint*>& getCheckpoints() const;
    const QList<QGraphicsItem*>& getObstaculosVisuais() const;
    Ponto2D getPosicaoLargada() const;

private:
    QList<Obstaculo> listaObstaculos; // Dados lógicos
    QList<QGraphicsItem*> obstaculosVisuais; // Hitbox real

    QList<Checkpoint*> listaCheckpoints;
    Ponto2D posicaoLargada;

    QPixmap imgIlha;
    QPixmap imgBoiaVermelha;
    QPixmap imgBoiaVerde;

    void criarIlha(QGraphicsScene *scene, float x, float y, float diametro);
    void criarBoia(QGraphicsScene *scene, float x, float y, QColor cor);
};

#endif // PISTA_H
