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

    // Corrida
    int voltaAtual;
    int proximoCheckpointId;

public:
    BarcoSolar(float x, float y);

    // Física
    void aplicarForca(Ponto2D forca);
    void aplicarHidrodinamica();
    void verificarLimitesMapa();
    void chocar();
    void atualizar(float deltaTime);

    // Controles
    void acelerar();
    void virar(float val);
    void carregarBateria();

    // IA / Automação
    void buscarAlvo(Ponto2D alvo);
    void incrementarCheckpoint(int totalCheckpoints);

    // Getters
    Ponto2D getPosicao() const;
    float getAngulo() const;
    float getBateria() const;
    float getVelocidadeAtual() const;
    int getProximoCheckpointId() const;
    int getVoltaAtual() const;
};

#endif // BARCOSOLAR_H
