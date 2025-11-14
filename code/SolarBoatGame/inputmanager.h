#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <QSet>
#include <QKeyEvent>

class InputManager
{
public:
    InputManager();

    void processarPressaoTecla(QKeyEvent *event);
    void processarSolturaTecla(QKeyEvent *event);
    bool isTeclaPressionada(int tecla) const;

private:
    QSet<int> teclasPressionadas;
};

#endif // INPUTMANAGER_H
