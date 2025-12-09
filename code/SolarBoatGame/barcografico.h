#ifndef BARCOGRAFICO_H
#define BARCOGRAFICO_H

#include "gameobjectgrafico.h"
#include <QPainter>
#include <QPixmap>
#include <QPainterPath>
#include <QVector>

class BarcoGrafico : public GameObjectGrafico
{
    Q_OBJECT
public:
    BarcoGrafico(QGraphicsItem *parent = nullptr);

    // Sincroniza visual com física
    void atualizarPosicaoTela(Ponto2D posicaoLogica, float anguloLogico) override;

    // Atualiza dados dos vetores de força
    void atualizarDebugInfo(Ponto2D motor, Ponto2D arrasto, Ponto2D resultante, float leme);
    void setDebug(bool ativado);

    // Define uma cor para diferenciar os barcos (máscara de cor)
    void setCor(QColor cor);

    // Define área de redesenho do item
    QRectF boundingRect() const override;
    // Define máscara precisa de colisão
    QPainterPath shape() const override;
    // Renderiza sprite e vetores de debug
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    // Cache de sprites rotacionados
    QVector<QPixmap> frames;
    // Cache de shapes para cada rotação
    QVector<QPainterPath> formas;
    int indiceAtual;

    // Estado visual
    bool debugEnabled;
    QColor corEquipe; // Cor da máscara
    bool usarCorPersonalizada;

    Ponto2D vMotor, vArrasto, vRes;
    float valLeme;
    float anguloLogicoAtual;
};

#endif // BARCOGRAFICO_H
