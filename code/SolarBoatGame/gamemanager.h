#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <QObject>
#include <QTimer>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QProgressBar>
#include <QLabel>
#include <QList>
#include <QGraphicsTextItem>

#include "barcojogador.h"
#include "barcooponente.h"
#include "barcografico.h"
#include "inputmanager.h"
#include "pista.h"

class GameManager : public QObject
{
    Q_OBJECT
public:
    GameManager(QGraphicsScene *scene, QGraphicsView *view,
                QProgressBar *bar, QLabel *labelVel, QLabel *labelVoltas,
                QObject *parent = nullptr);
    ~GameManager();

    void processarEvento(QKeyEvent *event, bool pressionado);

public slots:
    void gameLoop();

private:
    // Controle do estado do jogo
    void fimDeJogo(bool venceu);
    bool jogoAtivo;

    QGraphicsScene *scene;
    QGraphicsView *view;
    QProgressBar *barraBateria;
    QLabel *labelVelocidade;
    QLabel *labelVoltas;

    QTimer *timer;

    BarcoJogador *jogadorLogico;
    BarcoGrafico *jogadorVisual;

    QList<BarcoOponente*> oponentesLogicos;
    QList<BarcoGrafico*> oponentesVisuais;

    InputManager *inputManager;
    Pista *pista;

    Ponto2D posicaoAnteriorJogador;
};

#endif // GAMEMANAGER_H
