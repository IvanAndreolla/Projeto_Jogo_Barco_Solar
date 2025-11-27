#ifndef BARCOGRAFICO_H
#define BARCOGRAFICO_H

#include "gameobjectgrafico.h"
#include <QPainter>
#include <QPixmap>
#include <QPainterPath>

class BarcoGrafico : public GameObjectGrafico
{
    Q_OBJECT
public:
    BarcoGrafico(QGraphicsItem *parent = nullptr);

    void atualizarPosicaoTela(Ponto2D posicaoLogica, float anguloLogico) override;

    QRectF boundingRect() const override;

    // Este método diz ao Qt qual é a área clicável/colidível do objeto
    QPainterPath shape() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    QPixmap sprite;
    QPainterPath formaDeColisao; // Guarda o contorno exato da imagem
};

#endif // BARCOGRAFICO_H
