// Trabalho Prático
// SSC0140 - Sistemas Operacionais
// Professora: Kalinka Regina Lucas Jaquie Castelo Branco

// Grupo:
// Leonardo Ishida - 12873424
// Miguel Bragante Henriques - 13671894
// Nicholas Yudi Kurita Ikai - 13671852

#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <random>
#include <chrono>
#include <string>
#include <ncurses.h>

using namespace std;

vector<vector<int>> tabuleiro(5, vector<int>(40, 0));
vector<vector<int>> zumbis(5, vector<int>(40, 0));
vector<vector<int>> balas(5, vector<int>(40, 0));
mutex mtx_zumbi;
mutex mtx_bala;
mutex mtx_move;

char borda_hor[] = "+=======================================+========+";
int score = 0;
bool rodandoJogo = true;


bool randBool() {
    random_device rd; // Seed aleatório
    mt19937 gerador(rd());
    uniform_real_distribution<double> distribuicao(0.0, 1.0);  // Distribuição uniforme de 0 a 1

    return distribuicao(gerador) < 0.5;
}

void atualizarTabuleiro() {
    while (rodandoJogo) {
        {
            // Bloquear acesso as demais threads
            unique_lock<mutex> lock_b(mtx_bala);
            unique_lock<mutex> lock_z(mtx_zumbi);

            for (int i = 0; i < LINES-2; i++) {
                for (int j = 1; j < COLS-11; j++) {
                    
                    // Colidiu
                    if (zumbis[i][j] != 0 && balas[i][j] + zumbis[i][j] == 0) {
                        balas[i][j] = 0;
                        zumbis[i][j] = 0; 
                        score++;
                    }
                    else if (zumbis[i][j] != 0 && balas[i][j-1] + zumbis[i][j] == 0) {
                        balas[i][j-1] = 0;
                        zumbis[i][j] = 0; 
                        score++;
                    }

                    // Atualiza
                    tabuleiro[i][j] = balas[i][j] + zumbis[i][j];
                }
            }

            clear();  // Limpar tela

            mvprintw(0, 0, borda_hor); // Printar borda

            // Printar tabuleiro
            for (int i = 0; i < LINES-2; i++) {
                for (int j = 1; j < COLS-11; j++) {
                    mvprintw(i+1, j, "%d", tabuleiro[i][j]);
                }
            }

            // Desenhar as armas
            for (int i = 0; i < LINES-2; i++) {
                mvprintw(i+1, 0, ">");
            }

            mvprintw(6, 0, borda_hor); // Printar borda

            // Printar borda
            for (int i = 0; i < LINES-2; i++) {
                mvprintw(i+1, 40, "|");
            }

            // Printar placar
            mvprintw(2, 42, "Placar");
            mvprintw(4, 42, to_string(score).c_str());

            refresh();  // Atualizar a tela

            lock_b.unlock();
            lock_z.unlock();
        }

        // Aguarde um curto período para evitar alta taxa de atualização
        this_thread::sleep_for(chrono::milliseconds(100));
    }
}

void controleArma() {
    int ch;
    while (rodandoJogo) {
        {
            // Bloquear acesso as demais threads
            unique_lock<mutex> lock_b(mtx_bala);
            
            ch = getch();
            if (ch != ERR) {

                switch (ch) {
                    case 'q':
                        balas[0][1] = -1;
                        break;
                    case 'w':
                        balas[1][1] = -1;
                        break;
                    case 'e':
                        balas[2][1] = -1;
                        break;
                    case 'r':
                        balas[3][1] = -1;
                        break;
                    case 't':
                        balas[4][1] = -1;
                        break;
                }
            }

            lock_b.unlock();
        }
    }
}


void criaZumbis() {
    int baseDoZumbi = COLS - 12;

    while (rodandoJogo) {
        {
            // Bloquear acesso as demais threads
            unique_lock<mutex> lock_z(mtx_zumbi);
            
            // Criando zumbi novos
            for (int i = 0; i < LINES-2; i++) {
                // Usando booleano aleatório
                if (randBool()) {
                    // Spawna zumbi na linha i na base
                    zumbis[i][baseDoZumbi] = 1;
                }
            }

            lock_z.unlock();
        }

        // Aguarde um curto período antes de criar mais zumbis
        this_thread::sleep_for(chrono::seconds(1));
    }
}

void movePecas() {
    
    while (rodandoJogo) {
        {
            // Bloquear acesso as demais threads
            unique_lock<mutex> lock_z(mtx_zumbi);

            // Movendo todos os zumbis uma coluna para a esquerda
            for (int i = 0; i < LINES-2; i++) {
                for (int j = 1; j <= COLS-13; j++) {
                    // Swap
                    int aux = zumbis[i][j+1];
                    zumbis[i][j+1] = zumbis[i][j];
                    zumbis[i][j] = aux;
                }
                if(zumbis[i][1] == 1) { // Perdeu
                    rodandoJogo = false;
                }
            }
            lock_z.unlock();

            unique_lock<mutex> lock_b(mtx_bala);

            // Movendo as balas
            for (int i = 0; i < LINES-2; i++) {
                for (int j = COLS-12; j >= 1; j--) {
                    // Swap
                    int aux = balas[i][j-1];
                    balas[i][j-1] = balas[i][j];
                    balas[i][j] = aux;
                }
            }
            lock_b.unlock();
        }

        this_thread::sleep_for(chrono::seconds(1));
    }
}

int main() {

    initscr(); // Inicializar ncurses
    resize_term(7, 51); // Tamanho da tela
    timeout(0); // Tornar a entrada não bloqueante
    keypad(stdscr, TRUE); // Ativar leitura de teclas especiais
    noecho(); // Nao printar input na tela
    curs_set(0); // Nao aparecer o cursor a tela

    // Threads para controle
    thread threadTabuleiro(atualizarTabuleiro);
    thread threadArma(controleArma);
    thread threadCria(criaZumbis);
    thread threadMove(movePecas);
    
    // Fazendo threads funcionarem
    threadTabuleiro.join();
    threadArma.join();
    threadCria.join();
    threadMove.join();

    endwin(); // Encerrar ncurses

    printf("Perdeu! Score foi: %d\n", score);

    return 0;
}