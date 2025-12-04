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

    // Enumeração que define os layouts de pista e níveis de dificuldade disponíveis
    enum Tipo { OVAL_FACIL, OITO_MEDIO, SLALOM_DIFICIL };

    // Estrutura que armazena dados físicos dos obstáculos (posição e raio)
    // separando a lógica de colisão da representação visual
    struct Obstaculo {
        Ponto2D posicao;
        float raio;
    };

    // Limpa a cena atual e instancia os elementos da pista selecionada (ilhas, boias, checkpoints)
    void gerarCenario(QGraphicsScene *scene, Tipo tipo);

    // Verifica a colisão entre o item gráfico do barco e os obstáculos visuais.
    // Retorna true em caso de impacto e preenche 'posObstaculo' com a coordenada do objeto atingido para cálculo de física
    bool verificarColisaoVisual(QGraphicsItem *itemBarco, Ponto2D &posObstaculo);

    // Métodos de acesso (Getters) para permitir que o GameManager consulte o estado da pista
    const QList<Checkpoint*>& getCheckpoints() const;
    const QList<QGraphicsItem*>& getObstaculosVisuais() const;
    Ponto2D getPosicaoLargada() const;

private:
    QList<Obstaculo> listaObstaculos;     // Armazena dados lógicos para cálculos de física (colisão circular)
    QList<QGraphicsItem*> obstaculosVisuais; // Mantém referências aos itens gráficos renderizados na cena

    QList<Checkpoint*> listaCheckpoints;  // Lista sequencial de checkpoints/portões que o barco deve cruzar
    Ponto2D posicaoLargada;               // Define a coordenada inicial de spawn do barco

    // Recursos gráficos carregados na memória para otimização
    QPixmap imgIlha;
    QPixmap imgBoiaVermelha;
    QPixmap imgBoiaVerde;

    // Métodos auxiliares privados para instanciar objetos específicos na cena
    void criarIlha(QGraphicsScene *scene, float x, float y, float diametro);
    void criarBoia(QGraphicsScene *scene, float x, float y, QColor cor);
};

#endif // PISTA_H
