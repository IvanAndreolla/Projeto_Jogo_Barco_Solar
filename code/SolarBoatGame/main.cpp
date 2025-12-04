#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
    // Inicializa o gerenciador central da aplicação e recursos GUI
    QApplication a(argc, argv);

    // Configuração de Internacionalização (i18n)
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();

    // Itera sobre os idiomas preferidos do sistema para carregar o arquivo de tradução adequado
    for (const QString &locale : uiLanguages) {
        const QString baseName = "SolarBoatGame_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    // Instancia e exibe a janela principal do jogo
    MainWindow w;
    w.show();

    // Inicia o loop de processamento de eventos (bloqueante até o fechamento da janela)
    return a.exec();
}
