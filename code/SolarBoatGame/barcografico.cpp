#include "barcografico.h"
#include <QBitmap>

BarcoGrafico::BarcoGrafico(QGraphicsItem *parent)
    : GameObjectGrafico(parent)
{
    sprite.load(":/barco.png");

    if (!sprite.isNull()) {
        sprite = sprite.scaled(120, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        // --- PIXEL PERFECT COLLISION ---
        // 1. Extrai a máscara (pixels não transparentes)
        QBitmap mask = sprite.mask();
        // 2. Converte para região e depois para caminho vetorial
        QRegion region(mask);
        formaDeColisao.addRegion(region);

        // 3. Centraliza a forma para alinhar com o desenho (que é desenhado em -w/2, -h/2)
        QTransform trans;
        trans.translate(-sprite.width()/2, -sprite.height()/2);
        formaDeColisao = trans.map(formaDeColisao);
    } else {
        // Fallback se imagem falhar
        QPolygonF triangulo;
        triangulo << QPointF(20, 0) << QPointF(-10, -10) << QPointF(-5, 0) << QPointF(-10, 10);
        formaDeColisao.addPolygon(triangulo);
    }

    // O Z-Value será atualizado dinamicamente pelo GameManager
}

QRectF BarcoGrafico::boundingRect() const {
    return formaDeColisao.boundingRect();
}

QPainterPath BarcoGrafico::shape() const {
    return formaDeColisao;
}

void BarcoGrafico::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (sprite.isNull()) {
        painter->setBrush(QBrush(QColor("orange")));
        painter->setPen(QPen(Qt::black, 2));
        painter->drawPath(formaDeColisao);
    } else {
        painter->setRenderHint(QPainter::SmoothPixmapTransform);
        painter->drawPixmap(-sprite.width()/2, -sprite.height()/2, sprite);

        // Descomente para ver a hitbox visualmente para debug
        // painter->setPen(QPen(Qt::red, 1));
        // painter->drawPath(formaDeColisao);
    }
}

void BarcoGrafico::atualizarPosicaoTela(Ponto2D posicaoLogica, float anguloLogico) {
    // Conversão Isométrica
    float iso_x = posicaoLogica.x - posicaoLogica.y;
    float iso_y = (posicaoLogica.x + posicaoLogica.y) / 2.0f;

    setPos(iso_x, iso_y);

    // Rotação Visual (+180 se o sprite original aponta para a esquerda)
    float correcaoSprite = 180.0f;
    setRotation(anguloLogico + 45 + correcaoSprite);
}
