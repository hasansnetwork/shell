do: main.c
	gcc -I -Wall main.c main.h -o main
	./main

clean:
	rm main
	

