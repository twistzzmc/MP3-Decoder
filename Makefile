FLAGS = -ansi -pedantic -Wall -Wextra

all: compile run

compile:
	g++ $(FLAGS) -std=c++11 *.cpp -o mp3decoder

run:
	.\mp3decoder.exe mp3_files/670B.mp3

clean:
	rm mp3decoder.exe