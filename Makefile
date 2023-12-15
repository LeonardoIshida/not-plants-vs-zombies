all:
	g++ -o main src/main.cpp -lncurses -w

clean:
	rm main 

run:
	./main

