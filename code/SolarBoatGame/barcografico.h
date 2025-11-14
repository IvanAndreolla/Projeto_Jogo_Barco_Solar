#ifndef BARCOGRAFICO_H
#define BARCOGRAFICO_H

#include "gameobjectgrafico.h"
#include <QPainter>
#include <QPixmap>

class BarcoGrafico : public GameObjectGrafico
{
    Q_OBJECT
public:
    BarcoGrafico(QGraphicsItem *parent = nullptr);

    void atualizarPosicaoTela(Ponto2D posicaoLogica, float anguloLogico) override;

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    QPixmap sprite;
};

#endif // BARCOGRAFICO_H
