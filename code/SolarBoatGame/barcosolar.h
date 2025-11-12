#ifndef BARCOSOLAR_H
#define BARCOSOLAR_H

#include "ponto2d.h"

class BarcoSolar
{
private:
    // Física
    Ponto2D posicao;
    Ponto2D velocidade;
    Ponto2D aceleracao;
    float massa;
    float angulo; // Graus

    // Estado do Barco
    float nivelBateria;
    float capacidadeBateria;
    float taxaConsumo;
    float taxaRecarga;

    // Estado da Corrida
    int voltaAtual;
    int proximoCheckpointId;

public:
    BarcoSolar(float x, float y);

    // Métodos de Física e Lógica
    void aplicarForca(Ponto2D forca);
    void aplicarArrasto();
    void atualizar(float deltaTime);

    // Controles
    void acelerar();
    void virar(float val);
    void carregarBateria();

    // Getters
    Ponto2D getPosicao() const;
    float getAngulo() const;
    float getBateria() const;
};

#endif // BARCOSOLAR_H
