#ifndef BARCOSOLAR_H
#define BARCOSOLAR_H

#include "ponto2d.h"

class BarcoSolar
{
protected:
    // Física
    Ponto2D posicao;
    Ponto2D velocidade;
    Ponto2D aceleracao;

    float massa;
    float angulo;
    float velocidadeAngular;
    float aceleracaoAngular;

    // Estado
    float nivelBateria;
    float capacidadeBateria;
    float taxaConsumo;
    float taxaRecarga;
    float intensidadeSolar;

    // Corrida
    int voltaAtual;
    int proximoCheckpointId;

    // Colisão
    float raioColisao;

public:
    BarcoSolar(float x, float y);

    void resetar(float x, float y);
    void setIntensidadeSolar(float valor);

    void aplicarForca(Ponto2D forca);
    void aplicarHidrodinamica();
    void verificarLimitesMapa();

    //Recebe a posição do obstáculo para saber pra onde fugir
    void chocar(Ponto2D posObstaculo);

    void atualizar(float deltaTime);

    void acelerar();
    void virar(float val);
    void carregarBateria();

    void buscarAlvo(Ponto2D alvo);
    void incrementarCheckpoint(int totalCheckpoints);

    Ponto2D getPosicao() const;
    float getAngulo() const;
    float getBateria() const;
    float getVelocidadeAtual() const;
    int getProximoCheckpointId() const;
    int getVoltaAtual() const;
    float getRaioColisao() const;
};

#endif // BARCOSOLAR_H
