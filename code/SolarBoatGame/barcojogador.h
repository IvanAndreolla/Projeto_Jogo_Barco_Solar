#ifndef BARCOJOGADOR_H
#define BARCOJOGADOR_H

#include "barcosolar.h"
#include "inputmanager.h"

class BarcoJogador : public BarcoSolar
{
public:
    BarcoJogador(float x, float y, QString nome = "Jogador");
    void processarInput(InputManager *input);
};

#endif // BARCOJOGADOR_H
