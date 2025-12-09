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
#include <QFrame>

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
                QFrame *painel,
                QProgressBar *bar, QLabel *labelVel, QLabel *labelVoltas, QLabel *labelPos,
                QPushButton *btn1, QPushButton *btn2, QPushButton *btn3, QPushButton *btnReiniciar,
                QPushButton *btnRetomar, QPushButton *btnMenuPrincipal,
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
    void pausarJogo();
    void retomarJogo();
    void voltarAoMenu();

private:
    enum EstadoJogo { MENU, JOGANDO, GAMEOVER, PAUSADO };
    EstadoJogo estadoAtual;
    Pista::Tipo pistaAtualSelecionada;
    bool modoDebug;

    void iniciarJogo(Pista::Tipo tipo);
    void fimDeJogo(bool venceu);
    void atualizarInterface();
    void criarNuvens();
    void atualizarNuvens();
    void atualizarProfundidadeVisual();
    void atualizarRanking();

    QGraphicsScene *scene;
    QGraphicsView *view;
    QFrame *painelHUD;
    QProgressBar *barraBateria;
    QLabel *labelVelocidade;
    QLabel *labelVoltas;
    QLabel *labelPosicao;

    QPushButton *btnPista1;
    QPushButton *btnPista2;
    QPushButton *btnPista3;
    QPushButton *btnReiniciar;
    QPushButton *btnRetomar;
    QPushButton *btnMenuPrincipal;
    QLabel *labelTitulo;

    QTimer *timer;

    BarcoJogador *jogadorLogico;
    BarcoGrafico *jogadorVisual;

    QList<BarcoOponente*> oponentesLogicos;
    QList<BarcoGrafico*> oponentesVisuais;

    QList<BarcoSolar*> todosBarcos;
    QList<BarcoSolar*> barcosFinalizados;

    // Controlador de ordem de chegada para travar o placar
    int contadorChegada;

    struct Nuvem {
        QGraphicsPixmapItem* visual;
        float velocidade;
    };
    QList<Nuvem> listaNuvens;
    QPixmap texturaNuvem;

    InputManager *inputManager;
    Pista *pista;

    Ponto2D posicaoAnteriorJogador;
    QList<Ponto2D> posicoesAnterioresOponentes;
};

#endif // GAMEMANAGER_H
