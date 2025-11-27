#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <QObject>
#include <QTimer>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QProgressBar>
#include <QLabel>
#include <QPushButton>
#include <QList>
#include <QGraphicsTextItem>
#include <QGraphicsPixmapItem>

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
                QPushButton *btnJogar, QPushButton *btnReiniciar, QPushButton *btn3, QPushButton *btn4,
                QLabel *titulo,
                QObject *parent = nullptr);
    ~GameManager();

    void processarEvento(QKeyEvent *event, bool pressionado);

public slots:
    void gameLoop();
    void iniciarPista1();
    void iniciarPista2();
    void iniciarPista3();
    void reiniciarFase();

private:
    enum EstadoJogo { MENU, JOGANDO, GAMEOVER };
    EstadoJogo estadoAtual;
    Pista::Tipo pistaAtualSelecionada;

    void iniciarJogo(Pista::Tipo tipo);
    void fimDeJogo(bool venceu);
    void atualizarInterface();

    void criarNuvens();
    void atualizarNuvens();
    void atualizarProfundidadeVisual();

    QGraphicsScene *scene;
    QGraphicsView *view;

    QProgressBar *barraBateria;
    QLabel *labelVelocidade;
    QLabel *labelVoltas;
    QPushButton *btnPista1;
    QPushButton *btnPista2;
    QPushButton *btnPista3;
    QPushButton *btnReiniciar;
    QLabel *labelTitulo;

    QTimer *timer;

    BarcoJogador *jogadorLogico;
    BarcoGrafico *jogadorVisual;

    QList<BarcoOponente*> oponentesLogicos;
    QList<BarcoGrafico*> oponentesVisuais;

    struct Nuvem {
        QGraphicsPixmapItem* visual;
        float velocidade;
    };
    QList<Nuvem> listaNuvens;
    QPixmap texturaNuvem;

    InputManager *inputManager;
    Pista *pista;

    Ponto2D posicaoAnteriorJogador;
};

#endif // GAMEMANAGER_H
