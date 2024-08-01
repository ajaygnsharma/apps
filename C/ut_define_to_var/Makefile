main.o:
	gcc -c main.c
	  
main_02.o:
	gcc -c main_02.c  

main_03.o:
	gcc -c main_03.c  
	
	
all: main.o main_02.o main_03.o
	gcc -Wall -O0 -o main main.o
	gcc -Wall -O0 -o main_02 main_02.o
	gcc -Wall -O0 -o main_03 main_03.o
	
clean:
	rm main.o main_02.o main_03.o
	rm main main_02 main_03

