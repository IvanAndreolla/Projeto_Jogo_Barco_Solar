#ifndef BARCOSOLAR_H
#define BARCOSOLAR_H

#include "ponto2d.h"
#include <QString>

class BarcoSolar
{
protected:
    // Cinemática linear
    Ponto2D posicao;
    Ponto2D velocidade;
    Ponto2D aceleracao;

    // Dinâmica rotacional
    float massa;
    float angulo;
    float velocidadeAngular;
    float aceleracaoAngular;

    // Sistema de energia
    float nivelBateria;
    float capacidadeBateria;
    float taxaConsumo;
    float taxaRecarga;
    float intensidadeSolar;

    // Progresso na corrida
    int voltaAtual;
    int proximoCheckpointId;
    float raioColisao;

    // Identificação e flags
    QString nome;
    bool terminado;
    bool controleHabilitado;

    // Dados para debug visual
    Ponto2D debugForcaMotor;
    Ponto2D debugForcaArrasto;
    Ponto2D debugForcaResultante;
    float debugAnguloLeme;

public:
    BarcoSolar(float x, float y, QString nome);

    // Reinicia posição e física
    void resetar(float x, float y);
    void limparDebug();

    // Define eficiência do painel solar
    void setIntensidadeSolar(float valor);
    void setControle(bool habilitado);

    // Soma forças ao corpo (F=ma)
    void aplicarForca(Ponto2D forca);
    // Aplica arrasto da água
    void aplicarHidrodinamica();
    // Mantém barco na área jogável
    void verificarLimitesMapa();

    // Colisão com cenário (ricochete)
    void chocar(Ponto2D posObstaculo);
    // Colisão barco x barco
    void colidirComBarco(BarcoSolar* outroBarco);

    // Loop de atualização física
    void atualizar(float deltaTime);

    // Controles de movimento
    void acelerar();
    void virar(float val);
    void carregarBateria();

    // Navegação automática (IA)
    void buscarAlvo(Ponto2D alvo);
    // Lógica de progresso
    void incrementarCheckpoint(int totalCheckpoints);

    // Estado de fim de corrida
    void setTerminado(bool t);
    bool isTerminado() const;

    // Getters de estado
    Ponto2D getPosicao() const;
    float getAngulo() const;
    float getBateria() const;
    float getVelocidadeAtual() const;
    int getProximoCheckpointId() const;
    int getVoltaAtual() const;
    float getRaioColisao() const;
    QString getNome() const;

    // Pontuação para classificação
    int getScoreCorrida() const;

    // Getters de debug
    Ponto2D getDebugForcaMotor() const;
    Ponto2D getDebugForcaArrasto() const;
    Ponto2D getDebugForcaResultante() const;
    float getDebugAnguloLeme() const;
};

#endif // BARCOSOLAR_H
