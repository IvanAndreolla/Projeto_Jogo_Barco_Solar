#ifndef BARCOOPONENTE_H
#define BARCOOPONENTE_H

#include "barcosolar.h"

// Forward declaration para não precisar incluir o header pesado aqui
class Pista;

class BarcoOponente : public BarcoSolar
{
public:
    BarcoOponente(float x, float y);

    // IA decide para onde ir com base apenas na pista (checkpoints)
    void tomarDecisao(const Pista* pista);
};

#endif // BARCOOPONENTE_H
