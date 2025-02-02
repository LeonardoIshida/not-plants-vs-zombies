# NotPlants-vs-Zombies

Trabalho Prático da disciplina: SSC0140 - Sistemas Operacionais I.

O projeto propõe criar um jogo utilizando os conceitos vistos em aula acerca de threads e semáforos.
Dessa forma, optamos por criar um trabalho inspirado no famoso jogo Plants vs Zombies.

## 🤝 Contribuidores
[Miguel B. Henriques](https://github.com/MiguelHenri) <br>
[Nicholas Y. Ikai](https://github.com/niicao)

## Instalação
Primeiramente, devemos instalar a biblioteca ncurses e o git:
#### Ubuntu, Debian, Mint
```
sudo apt install libncurses5-dev libncursesw5-dev git
```
#### Arch
```
sudo pacman -S ncurses git
```

Agora, vamos clonar o repositório:
```
git clone https://github.com/LeonardoIshida/plants-vs-zombies.git
```

Por fim, vamos compilar o jogo:
```
cd plants-vs-zombies && make
```

## Como jogar ?
### 🎮 Iniciar o jogo
```
make run
```
### 🔫 Comandos
Os síbolos '>' representam as armas utilizadas para atirar nos zumbis, e cada arma é ativada por uma tecla:  
- 'Q': atirar  na primeira linha;
- 'W': atirar  na segunda linha; 
- 'E': atirar  na terceira linha; 
- 'R': atirar  na quarta linha;  
- 'T': atirar  na quinta linha.  

### 🏆 Objetivos
Conseguir obter a maior pontuação possível e não deixar os zumbis chegarem onde estão as armas.

## Implementação

### 🧵 Threads
As threads foram implementadas para que o nosso jogo não seja sequencial, pois estavamos enfrentando os seguintes problemas com o código sequencial:
- Input do usuário bloqueando as outras ações do jogo.
- Verificação de colisão entre as balas e os zumbis deve ser feita de maneira muito mais frequente do que a criação de novos zumbis, porque as balas são criadas instantâneamente.
- Mover as peças deve ser independente do input do usuário e da criação dos zumbis, em questão de ordem de execução.

Dessa maneira, a solução foi utilizar threads. Foram utilizados 4 threads para as seguintes funções: atualizarTabuleiro, controleArma, criaZumbis e movePecas.
Agora, com as 4 funções sendo executadas 'ao mesmo tempo' e com auxílio das funções sleep foi possível resolver os problemas apresentados pelo programa sequencial:
- controleArma: conseguimos receber input do usuário no momento em que a tecla for pressionada, e caso não ocorra nenhuma entrada os zumbis continuam spawnando e andando pelo mapa.
- atualizarTabuleiro: verificamos se houve contato entre as balas e os zumbis mais frequentemente, o que é necessário devido ao fato do input do usuário ser 'instantâneo' e a movimentação dos zumbis não.
- criaZumbis e movePecas: criamos novos zumbis e movemos todos os objetos do tabuleiro, cada um com sua periodicidade ideal.
- sleep: essencial para sincronizar a frequência das execuções das threads, dessa forma, conseguimos executar as threads citadas em seu tempo de frequência ótimo para uma melhor experiência de jogo. Foi a parte mais difícil do projeto, pois não tínhamos nenhuma experiência e foi resolvido fazendo vários testes.

### 🚦 Mutex

Como o nosso programa é executado por várias threads ao mesmo tempo, estávamos enfrentando o problema de Race condition, em que várias threads estavam tentando acessar o mesmo espaço de memória. Dessa forma, foi de extrema importância utlizar mutex para coordenar os acessos às regiões críticas e não gerar resultados estranhos e inconsistentes.
- Mutex zumbi: foi usado para gerenciar o acesso nas funções atualizarTabuleiro, criaZumbis e movePecas. Quando uma dessas funções é executada, precisamos garantir que as outras não acessem a matriz "zumbis", pois qualquer interferência poderia ocasionar no mau funcionamento do processo. Se, por exemplo, moverPecas e criaZumbis acessassem a mesma posição de memória ao mesmo tempo, teríamos problemas para saber se deveríamos adicionar zumbis em uma certa posição ou se deveríamos mover o conteúdo para 'frente'. Portanto, para resolver esse e outros problemas, utilizamos a classe unique_lock, que, quando utilizada sobre um mutex, bloqueia o acesso por outras threads até que seja feito um unlock. Então, antes de começarmos a manipular qualquer dado presente na matriz "zumbis", fizemos o unique_lock para efetuar o acesso seguro e não simultâneo da variável. Assim, garantimos que a função reproduza sempre o resultado desejado.
- Mutex bala: foi usado para gerenciar o acesso nas funções atualizarTabuleiro, controleArma e movePecas. Aqui, temos o problema de que queremos receber o input do usuário, mas, ao mesmo tempo, precisamos mover as balas, que já estão no tabuleiro, para 'frente', e precisamos verificar as colisões entre as balas e os zumbis. Portanto, para gerenciar o acesso dessas 3 funções, também utilizamos aqui a classe unique_lock, para garantir que uma thread não atrapalhe o funcionamento das outras, bloqueando o acesso à variável, e ao terminar de executar a rotina, liberando o acesso com unlock para outras threads acessarem. Dessa maneira, conseguimos fazer com que o input do usuário ocorra 'instantâneamente' e não cause problemas para as funções que também têm que manipular informações presentes em "balas".

## Link para o vídeo
https://www.youtube.com/watch?v=9j03Oe1_E-I
