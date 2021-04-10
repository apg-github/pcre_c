#include <stdio.h>
#include <stdlib.h>

void zadanie1()
{
//TUTAJ KOD ZADANIA 1
printf("Uruchomiono zad 1 ...");
}

void zadanie2()
{
//TUTAJ KOD ZADANIA 2
printf("Uruchomiono zad 2 ...");
}

void zadanie3()
{
//TUTAJ KOD ZADANIA 3
printf("Uruchomiono zad 3 ...");
}

int main(int argc, char *argv[]) 
{
    if (argc == 2)
    {
        if (atoi(argv[1]) == 1) {
        	
			zadanie1();	
        	return 0;
        }
        if (atoi(argv[1]) == 2) {
        	zadanie2();
        	return 0;
        }
        if (atoi(argv[1]) == 3) {
        	zadanie3();
        	return 0;
        }
    } else {
    	printf("Podaj numer zadania!");
    }

}
