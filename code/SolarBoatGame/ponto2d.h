#ifndef PONTO2D_H
#define PONTO2D_H

#include <cmath>

class Ponto2D
{
public:
    float x;
    float y;

    Ponto2D();
    Ponto2D(float x_, float y_);

    void add(Ponto2D v);
    void mult(float n);
    float mag() const;
    void normalize();
    void limit(float max);
    float dist(Ponto2D v) const;

    Ponto2D operator+(const Ponto2D& v) const;
    Ponto2D operator-(const Ponto2D& v) const;
    Ponto2D operator*(float n) const;
    Ponto2D operator/(float n) const;
};

#endif // PONTO2D_H
