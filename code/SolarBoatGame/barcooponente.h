#ifndef BARCOOPONENTE_H
#define BARCOOPONENTE_H

#include "barcosolar.h"

class Pista; // Forward declaration

class BarcoOponente : public BarcoSolar
{
public:
    BarcoOponente(float x, float y);

    // IA decide para onde ir com base na pista
    void tomarDecisao(const Pista* pista);
};

#endif // BARCOOPONENTE_H
