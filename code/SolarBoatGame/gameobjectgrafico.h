#ifndef GAMEOBJECTGRAFICO_H
#define GAMEOBJECTGRAFICO_H

#include <QGraphicsObject>
#include "ponto2d.h"

class GameObjectGrafico : public QGraphicsObject
{
    Q_OBJECT
public:
    GameObjectGrafico(QGraphicsItem *parent = nullptr) : QGraphicsObject(parent) {}

    // Método virtual puro: Obriga filhos a implementarem a projeção
    virtual void atualizarPosicaoTela(Ponto2D posicaoLogica, float anguloLogico) = 0;
};

#endif // GAMEOBJECTGRAFICO_H
