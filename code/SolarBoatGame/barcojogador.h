#ifndef BARCOJOGADOR_H
#define BARCOJOGADOR_H

#include "barcosolar.h"
#include "inputmanager.h"
#include "pista.h"

class BarcoJogador : public BarcoSolar
{
public:
    BarcoJogador(float x, float y, QString nome = "Jogador");

    // Implementação Polimórfica: O jogador usa o InputManager
    void controlar(InputManager* input, const Pista* pista) override;

    void processarInput(InputManager *input);
};

#endif // BARCOJOGADOR_H
