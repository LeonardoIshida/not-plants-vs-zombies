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
vector<vector<int>> zumbis(5, vector<int>(40, 0));
vector<vector<int>> balas(5, vector<int>(40, 0));
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
            
            clear();  // Limpar tela

            for (int i = 0; i < LINES; i++) {
                for (int j = 1; j < COLS; j++) {
                    mvprintw(i, j, "%d", tabuleiro[i][j]);
                }
            }
            
            // Desenhar as armas
            for (int i = 0; i < LINES; i++) {
                mvprintw(i, 0, ">");
            }

            refresh();  // Atualizar a tela
        }

        // Aguarde um curto período para evitar alta taxa de atualização
        this_thread::sleep_for(chrono::milliseconds(50));
    }
}

void verificaColisao() {

    while (true) {
        {
            lock_guard<mutex> lock(mtx);
            lock_guard<mutex> lock_b(mtx_bala);
            lock_guard<mutex> lock_z(mtx_zumbi);

            for (int i = 0; i < LINES; i++) {
                for (int j = 1; j < COLS; j++) {
                    // Colidiu
                    if (balas[i][j] + zumbis[i][j] == 0) {
                        balas[i][j] = 0;
                        zumbis[i][j] = 0; 
                    }
                    // Atualiza
                    tabuleiro[i][j] = balas[i][j] + zumbis[i][j];
                }
            }
        }

        this_thread::sleep_for(chrono::seconds(1));   
    }

}

void controleArma() {
    int ch, x = 0, y = 0;
    while (true) {
        {
            lock_guard<mutex> lock_b(mtx_bala);
            // lock_guard<mutex> lock(mtx);
            ch = getch();
                
            if (ch != ERR) {

                switch (ch) {
                    case 'q':
                        // balas.push_back(make_pair(0, 0));
                        balas[0][1] -= 1;
                        break;
                    case 'w':
                        // balas.push_back(make_pair(1, 0));
                        balas[1][1] -= 1;
                        break;
                    case 'e':
                        // balas.push_back(make_pair(2, 0));
                        balas[2][1] -= 1;
                        break;
                    case 'r':
                        // balas.push_back(make_pair(3, 0));
                        balas[3][1] -= 1;
                        break;
                    case 't':
                        // balas.push_back(make_pair(4, 0));
                        balas[4][1] -= 1;
                        break;
                }
            }
        }
    }
}


void criaZumbis() {
    int baseDoZumbi = COLS - 1;

    while (true) {
        {
            lock_guard<mutex> lock_z(mtx_zumbi);
            
            // Criando zumbi novos
            for (int i = 0; i < LINES; i++) {
                // Usando booleano aleatório
                if (randBool()) {
                    // Spawna zumbi na linha i na base
                    zumbis[i][baseDoZumbi] = 1;
                }
            }
        }

        // Aguarde um curto período antes de criar mais zumbis
        this_thread::sleep_for(chrono::seconds(2));
    }
}

void moveZumbi() {
    
    while (true) {
        {
            lock_guard<mutex> lock_z(mtx_zumbi);

            // Movendo todos os zumbis uma coluna para a esquerda
            for (int i = 0; i < LINES; i++) {
                for (int j = 1; j < COLS-1; j++) {
                    // Swap
                    int aux = zumbis[i][j+1];
                    zumbis[i][j+1] = zumbis[i][j];
                    zumbis[i][j] = aux;
                }
                // tabuleiro[pos.first][pos.second] += 1;
                // if (pos.second + 1 < LINES) {
                //     pos.second += 1;
                //     tabuleiro[pos.first][pos.second] -= 1;
                // }
            }
        }

        this_thread::sleep_for(chrono::seconds(2));
    }
}

void moveBala() {
    
    while (true) {
        {
            // lock_guard<mutex> lock(mtx);
            lock_guard<mutex> lock_b(mtx_bala);

            // Movendo as balas
            for (int i = 0; i < LINES; i++) {
                for (int j = COLS-1; j >= 1; j--) {
                    // Swap
                    int aux = balas[i][j-1];
                    balas[i][j-1] = balas[i][j];
                    balas[i][j] = aux;
                }
                // tabuleiro[pos.first][pos.second] += 1;
                // if (pos.second + 1 < LINES) {
                //     pos.second += 1;
                //     tabuleiro[pos.first][pos.second] -= 1;
                // }
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
    thread threadCria(criaZumbis);
    thread threadBala(moveBala);
    thread threadMove(moveZumbi);
    thread threadColisao(verificaColisao);
    
    // Aguarde threads terminarem
    threadTabuleiro.join();
    threadArma.join();
    threadCria.join();
    threadBala.join();
    threadMove.join();
    threadColisao.join();

    endwin();  // Encerrar ncurses

    return 0;
}
