#include "barcografico.h"
#include <QBitmap>
#include <cmath>
#include <QDebug>

const int TOTAL_FRAMES = 64;
const float GRAUS_POR_FRAME = 360.0f / TOTAL_FRAMES;

// AJUSTE: 225 graus para alinhar sprites com física
const float CALIBRACAO_OFFSET = 225.0f;

BarcoGrafico::BarcoGrafico(QGraphicsItem *parent)
    : GameObjectGrafico(parent)
{
    indiceAtual = 0;
    debugEnabled = false; // Inicia desligado, F1 liga

    vMotor = Ponto2D(0,0); vArrasto = Ponto2D(0,0); vRes = Ponto2D(0,0);
    valLeme = 0.0f;
    anguloLogicoAtual = 0.0f;

    for (int i = 0; i < TOTAL_FRAMES; ++i) {
        QString nomeArquivo = QString(":/barco_%1.png").arg(i);
        QPixmap img(nomeArquivo);

        if (img.isNull()) {
            if (!frames.isEmpty()) img = frames[0];
            else img = QPixmap(10, 10);
        }

        img = img.scaled(160, 160, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        frames.append(img);

        QBitmap mask = img.mask();
        QRegion region(mask);
        QPainterPath path;
        path.addRegion(region);
        QTransform trans;
        trans.translate(-img.width()/2, -img.height()/2);
        path = trans.map(path);
        formas.append(path);
    }
    setZValue(10);
}

QRectF BarcoGrafico::boundingRect() const {
    if (formas.isEmpty()) return QRectF();
    return formas[indiceAtual].boundingRect();
}

QPainterPath BarcoGrafico::shape() const {
    if (formas.isEmpty()) return QPainterPath();
    return formas[indiceAtual];
}

void BarcoGrafico::setDebug(bool ativado) {
    debugEnabled = ativado;
    update();
}

void BarcoGrafico::atualizarDebugInfo(Ponto2D motor, Ponto2D arrasto, Ponto2D resultante, float leme) {
    vMotor = motor;
    vArrasto = arrasto;
    vRes = resultante;
    valLeme = leme;
    if (debugEnabled) update();
}

void BarcoGrafico::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (frames.isEmpty()) return;

    QPixmap& spriteAtual = frames[indiceAtual];
    painter->setRenderHint(QPainter::SmoothPixmapTransform);
    painter->drawPixmap(-spriteAtual.width()/2, -spriteAtual.height()/2, spriteAtual);

    if (debugEnabled) {
        auto toIso = [](float x, float y) { return QPointF(x - y, (x + y) / 2.0f); };

        float rad = anguloLogicoAtual * (3.14159f / 180.0f);
        float cosA = std::cos(rad);
        float sinA = std::sin(rad);

        QPointF p1 = toIso(cosA * 80, sinA * 80);
        QPointF p2 = toIso(cosA * -30 - sinA * 20, sinA * -30 + cosA * 20);
        QPointF p3 = toIso(cosA * -30 + sinA * 20, sinA * -30 - cosA * 20);

        QPolygonF tri; tri << p1 << p2 << p3;
        painter->setBrush(Qt::NoBrush);
        painter->setPen(QPen(Qt::white, 2, Qt::DotLine));
        painter->drawPolygon(tri);

        // Vetores
        float scaleMotor = 0.01f;
        if (vMotor.mag() > 10.0f) {
            painter->setPen(QPen(Qt::green, 3));
            painter->drawLine(QPointF(0,0), toIso(vMotor.x*scaleMotor, vMotor.y*scaleMotor));
        }

        float scaleArrasto = 0.002f;
        if (vArrasto.mag() > 10.0f) {
            painter->setPen(QPen(Qt::red, 2));
            painter->drawLine(QPointF(0,0), toIso(vArrasto.x*scaleArrasto, vArrasto.y*scaleArrasto));
        }

        float scaleRes = 0.002f;
        if (vRes.mag() > 10.0f) {
            painter->setPen(QPen(Qt::cyan, 2));
            painter->drawLine(QPointF(0,0), toIso(vRes.x*scaleRes, vRes.y*scaleRes));
        }

        // Hitbox
        painter->setPen(QPen(Qt::yellow, 2, Qt::DashLine));
        painter->drawEllipse(QPointF(0, 0), 8.0f, 4.8f);
    }
}

void BarcoGrafico::atualizarPosicaoTela(Ponto2D posicaoLogica, float anguloLogico) {
    float iso_x = posicaoLogica.x - posicaoLogica.y;
    float iso_y = (posicaoLogica.x + posicaoLogica.y) / 2.0f;
    setPos(iso_x, iso_y);
    setRotation(0);

    anguloLogicoAtual = anguloLogico;

    float angulo = anguloLogico;
    while (angulo < 0) angulo += 360.0f;
    while (angulo >= 360) angulo -= 360.0f;

    // Mapeamento Linear
    float anguloFinal = angulo + CALIBRACAO_OFFSET;

    int index = (int)std::round(anguloFinal / GRAUS_POR_FRAME);
    index = index % TOTAL_FRAMES;
    if (index < 0) index += TOTAL_FRAMES;

    if (index >= 0 && index < frames.size()) {
        if (indiceAtual != index) {
            indiceAtual = index;
            prepareGeometryChange();
        }
    }
}
