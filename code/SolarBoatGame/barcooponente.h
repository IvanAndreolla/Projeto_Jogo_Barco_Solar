#ifndef BARCOOPONENTE_H
#define BARCOOPONENTE_H

#include "barcosolar.h"

class Pista;

class BarcoOponente : public BarcoSolar
{
public:
    BarcoOponente(float x, float y, QString nome);
    void tomarDecisao(const Pista* pista);
};

#endif // BARCOOPONENTE_H
