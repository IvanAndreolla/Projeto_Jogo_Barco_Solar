#include "barcografico.h"

BarcoGrafico::BarcoGrafico(QGraphicsItem *parent)
    : GameObjectGrafico(parent)
{
    // Carrega a imagem
    sprite.load(":/barco.png");

    if (!sprite.isNull()) {
        sprite = sprite.scaled(120, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    setZValue(10);
}

QRectF BarcoGrafico::boundingRect() const {
    if (sprite.isNull()) return QRectF(-20, -20, 40, 40);

    return QRectF(-sprite.width()/2, -sprite.height()/2,
                  sprite.width(), sprite.height());
}

void BarcoGrafico::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (sprite.isNull()) {
        QPolygonF triangulo;
        triangulo << QPointF(20, 0) << QPointF(-10, -10) << QPointF(-5, 0) << QPointF(-10, 10);
        painter->setBrush(QBrush(QColor("orange")));
        painter->setPen(QPen(Qt::black, 2));
        painter->drawPolygon(triangulo);
    } else {
        painter->setRenderHint(QPainter::SmoothPixmapTransform);
        painter->drawPixmap(-sprite.width()/2, -sprite.height()/2, sprite);
    }
}

void BarcoGrafico::atualizarPosicaoTela(Ponto2D posicaoLogica, float anguloLogico) {
    // Projeção Isométrica
    float iso_x = posicaoLogica.x - posicaoLogica.y;
    float iso_y = (posicaoLogica.x + posicaoLogica.y) / 2.0f;

    setPos(iso_x, iso_y);

    float correcaoSprite = 180.0f;

    setRotation(anguloLogico + 45 + correcaoSprite);
}
