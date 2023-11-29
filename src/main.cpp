#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <random>
#include <chrono>
#include <ncurses.h>
// #include "tabuleiro.h"

using namespace std;

vector<int> zumbis;
mutex mtx;
int a = 0;
void controleArma();

void atualizarTabuleiro() {
    while (true) {
        {
            // lock_guard<mutex> lock(mtx);
            // Lógica para atualizar o tabuleiro

            // Movimentando os zumbis

            // Por exemplo, movimentar os zumbis e verificar colisões

            clear();  // Limpar tela
            
            // Desenhar os zumbis, arma, etc. no terminal
            // for (int pos : zumbis) {
            //     mvprintw(0, pos, "Z");
            // }

            // Desenhar as armas
            for (int i = 0; i < LINES; i++) {
                mvprintw(i, 0, "%d", a);
            }

            refresh();  // Atualizar a tela
        }

        // Aguarde um curto período para evitar alta taxa de atualização
        this_thread::sleep_for(chrono::milliseconds(100));

        // Atualizar aqui
    }
}

void controleArma() {
    keypad(stdscr, TRUE);  // Ativar leitura de teclas especiais
    noecho();
    
    int ch = getch();
    while(ch != 27){
        //shoot()
        printw("Tecla pressionada %c\n", ch);
        refresh();
    }

}

void controleZumbis() {
    int baseDoZumbi = COLS - 1;

    while (true) {
        {
            lock_guard<mutex> lock(mtx);
            // Lógica para liberação e movimentação dos zumbis
            
            // Criando zumbi
            

            // Por exemplo, adicionar zumbis à lista
        }

        // Aguarde um curto período antes de liberar mais zumbis
        this_thread::sleep_for(chrono::milliseconds(1000));
    }
}

int main() {
    initscr();  // Inicializar ncurses
    resize_term(5, 11); // Tamanho da tela
    timeout(0); // Tornar a entrada não bloqueante
    keypad(stdscr, TRUE);  // Ativar leitura de teclas especiais
    // controleArma();



    // Threads para controle
    thread threadTabuleiro(atualizarTabuleiro);
    thread threadArma(controleArma);
    // thread threadZumbis(controleZumbis);

    // Aguarde threads terminarem
    threadTabuleiro.join();
    threadArma.join();
    // threadZumbis.join();

    endwin();  // Encerrar ncurses

    return 0;
}
