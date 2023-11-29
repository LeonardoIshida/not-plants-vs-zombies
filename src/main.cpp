#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <random>
#include <chrono>
#include <ncurses.h>
// #include "tabuleiro.h"

using namespace std;

// TO-DO:
//  Modificar os sleeps, sincronizar
//  Separar em mais threads
//  

vector<vector<int>> tabuleiro(5, vector<int>(40, 0));
vector<pair<int, int>> zumbis;
vector<pair<int, int>> balas;
mutex mtx;
mutex mtx_zumbi;
mutex mtx_bala;

bool randBool() {
    random_device rd; // Seed aleatório
    mt19937 gerador(rd());
    uniform_real_distribution<double> distribuicao(0.0, 1.0);  // Distribuição uniforme de 0 a 1

    return distribuicao(gerador) < 0.5;
}

void atualizarTabuleiro() {
    while (true) {
        {
            lock_guard<mutex> lock(mtx);
            
            // Verificar colisões
            

            clear();  // Limpar tela

            for (int i = 0; i < LINES; i++) {
                for (int j = 1; j < COLS; j++)
                    mvprintw(i, j, "%d", tabuleiro[i][j-1]);
            }

            // Desenhar as armas
            for (int i = 0; i < LINES; i++) {
                mvprintw(i, 0, "L");
            }

            refresh();  // Atualizar a tela
        }

        // Aguarde um curto período para evitar alta taxa de atualização
        this_thread::sleep_for(chrono::milliseconds(100));
    }
}

void controleArma() {

    int ch, x = 0, y = 0;
    while (true) {
        {
            ch = getch();
                
            if (ch != ERR) {
                lock_guard<mutex> lock(mtx);
                lock_guard<mutex> lock_b(mtx_bala);

                switch (ch) {
                    case 'q':
                        balas.push_back(make_pair(0, 0));
                        tabuleiro[0][0] -= 1;
                        break;
                    case 'w':
                        balas.push_back(make_pair(1, 0));
                        tabuleiro[1][0] -= 1;
                        break;
                    case 'e':
                        balas.push_back(make_pair(2, 0));
                        tabuleiro[2][0] -= 1;
                        break;
                    case 'r':
                        balas.push_back(make_pair(3, 0));
                        tabuleiro[3][0] -= 1;
                        break;
                    case 't':
                        balas.push_back(make_pair(4, 0));
                        tabuleiro[4][0] -= 1;
                        break;
                }
            }
        }

        // Aguarde um curto período para evitar alta taxa de atualização
        this_thread::sleep_for(chrono::milliseconds(50));

        {
            lock_guard<mutex> lock(mtx);
            lock_guard<mutex> lock_b(mtx_bala);

            // Movendo as balas
            for (pair<int, int> &pos : balas) {
                tabuleiro[pos.first][pos.second] += 1;
                pos.second += 1;
                tabuleiro[pos.first][pos.second] -= 1;
            }
        }

        this_thread::sleep_for(chrono::seconds(1));
        
    }
}


void controleZumbis() {
    int baseDoZumbi = COLS - 1;

    while (true) {
        {
            lock_guard<mutex> lock(mtx);
            lock_guard<mutex> lock_z(mtx_zumbi);
            
            // Criando zumbi novos
            for (int i = 0; i < LINES; i++) {
                // Usando booleano aleatório
                if (randBool()) {
                    // Spawna zumbi na linha i na base
                    zumbis.push_back(make_pair(i, baseDoZumbi));
                }
            }
        }

        // Aguarde um curto período antes de criar mais zumbis
        this_thread::sleep_for(chrono::seconds(2));

        {
            lock_guard<mutex> lock(mtx);
            lock_guard<mutex> lock_z(mtx_zumbi);

            // Movendo todos os zumbis uma coluna para a esquerda
            for (pair<int, int> &pos : zumbis) {
                tabuleiro[pos.first][pos.second] -= 1;
                pos.second -= 1;
                tabuleiro[pos.first][pos.second] += 1;
            }
        }

        this_thread::sleep_for(chrono::seconds(1));
    }
}

int main() {
    initscr();  // Inicializar ncurses
    resize_term(5, 40); // Tamanho da tela
    timeout(0); // Tornar a entrada não bloqueante
    keypad(stdscr, TRUE);  // Ativar leitura de teclas especiais
    noecho();
    curs_set(0);

    // Threads para controle
    thread threadTabuleiro(atualizarTabuleiro);
    thread threadArma(controleArma);
    thread threadZumbis(controleZumbis);

    // Aguarde threads terminarem
    threadTabuleiro.join();
    threadArma.join();
    threadZumbis.join();

    endwin();  // Encerrar ncurses

    return 0;
}
