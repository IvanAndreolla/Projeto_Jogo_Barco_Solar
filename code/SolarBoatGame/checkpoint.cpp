#include "checkpoint.h"

Checkpoint::Checkpoint(int id, Ponto2D p1, Ponto2D p2) : id(id), p1(p1), p2(p2) {}

int Checkpoint::getId() const { return id; }
Ponto2D Checkpoint::getP1() const { return p1; }
Ponto2D Checkpoint::getP2() const { return p2; }

Ponto2D Checkpoint::getCentro() const {
    return (p1 + p2) * 0.5f;
}

bool Checkpoint::cruzouLinha(Ponto2D posAntiga, Ponto2D posNova) const {
    auto ccw = [](Ponto2D a, Ponto2D b, Ponto2D c) {
        return (c.y - a.y) * (b.x - a.x) > (b.y - a.y) * (c.x - a.x);
    };
    return (ccw(p1, posAntiga, posNova) != ccw(p2, posAntiga, posNova)) &&
           (ccw(p1, p2, posAntiga) != ccw(p1, p2, posNova));
}
