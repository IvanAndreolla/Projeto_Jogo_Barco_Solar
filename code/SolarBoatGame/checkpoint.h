#ifndef CHECKPOINT_H
#define CHECKPOINT_H

#include "ponto2d.h"

class Checkpoint
{
public:
    Checkpoint(int id, Ponto2D p1, Ponto2D p2);

    int getId() const;
    Ponto2D getP1() const;
    Ponto2D getP2() const;
    Ponto2D getCentro() const;

    bool cruzouLinha(Ponto2D posAntiga, Ponto2D posNova) const;

private:
    int id;
    Ponto2D p1;
    Ponto2D p2;
};

#endif // CHECKPOINT_H
