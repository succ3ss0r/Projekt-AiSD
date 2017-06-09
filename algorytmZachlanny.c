#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>

#define MAXELEMENTS 30

struct punkt {
    //struktura potrzebna do stworzenia listy punktow która zostanie przerobiona na tablicę przejść lub listy przejść
    int wspX;
    int wspY;
    bool odwiedzony;
    struct punkt *nastepny;
};
void addPoint(struct punkt *listaPunktow, int x, int y) {
    //dodaje tyle punktow ile chce użytkownik
    while(listaPunktow->nastepny)
        listaPunktow = listaPunktow->nastepny;

    struct punkt *tmp = (struct punkt *) malloc(sizeof(struct punkt)); //zarezerwowanie pamięci

    if(tmp) {
        //jeżeli poprawnie zalokowano pamięć

        //usupełnij współrzedne nowego punktu
        tmp->wspX = x;
        tmp->wspY = y;

        tmp->nastepny = listaPunktow->nastepny;
        listaPunktow->nastepny = tmp; // przepnij wskaznik listy na poczatek
    } else {
        //nie zalokowano pamięci

        fprintf(stderr, "\nNie udalo sie zaalokowac pamieci!");
        return;
    }
}
int showPoints(struct punkt *listaPunktow) {
    //wyświetlanie elementów listy, zwraca numer porządkowy ostatniego elementu

    int number = 1; //numery porządkowe od 1

    if(!listaPunktow->nastepny) {
        //jeżeli lista jest pusta wyświetl ostrzeżenie
        printf("\n\tBrak punktow! Najpierw dodaj ich kilka.");
    } else {
        //jeżeli lista zawiera elementy

        while(listaPunktow->nastepny) {
            //dopkóki element listy wskazuje na następny

            listaPunktow = listaPunktow->nastepny; //pierwszy element jest pusty dlatego trzeba przepiąć go teraz
            printf("\n\t%d. (%d, %d)", number, listaPunktow->wspX, listaPunktow->wspY);

            ++number; //zwiększ numer porzadkowy
        }
    }

    return number - 1; //zwroc numer porzadkowy ostatniego elementu
}
double calculateDistance(struct punkt *miasto1, struct punkt *miasto2) {
    double X = pow(miasto1->wspX - miasto2->wspX, 2);
    double Y = pow(miasto1->wspY - miasto2->wspY, 2);
    return sqrt(X + Y);
}
void algorithmGreedy(struct punkt *listaPunktow, struct punkt *listaZachlanny, double *dlugoscZachlanny) {
    //algorytm zachlanny
    struct punkt *aktualnieOdwiedzane = listaPunktow->nastepny;
    aktualnieOdwiedzane->odwiedzony = true;
    addPoint(listaZachlanny, aktualnieOdwiedzane->wspX, aktualnieOdwiedzane->wspY);

    struct punkt *miasto1, *miasto2;

    listaPunktow = listaPunktow->nastepny;

    struct punkt *listaPunktow_CP = listaPunktow;
    struct punkt *poczatekListy = listaPunktow;
    while(listaPunktow_CP->nastepny) {
        listaPunktow_CP = listaPunktow_CP->nastepny;
        miasto1 = miasto2 = NULL;
        listaPunktow = poczatekListy;
        while(listaPunktow){
            if(listaPunktow->odwiedzony) {
                printf("\nPrzeskoczenie");
                listaPunktow = listaPunktow->nastepny;
                continue;
            }
            if(!miasto1) {
                printf("\nBrak miasto1");
                miasto1 = listaPunktow;
                listaPunktow = listaPunktow->nastepny;
                continue;
            }
            if(!miasto2) {
                printf("\nBrak miasto2\n");
                miasto2 = listaPunktow;
                listaPunktow = listaPunktow->nastepny;
                continue;
            }
            if(calculateDistance(aktualnieOdwiedzane, miasto1) > calculateDistance(aktualnieOdwiedzane, miasto2))
                miasto1 = miasto2;
            miasto2 = listaPunktow;
            listaPunktow = listaPunktow->nastepny;


            printf("Miasto1: %d, %d \tMiasto2: %d, %d\n", miasto1->wspX, miasto1->wspY, miasto2->wspX, miasto2->wspY);
        }
        addPoint(listaZachlanny, miasto1->wspX, miasto1->wspY);
        miasto1->odwiedzony = true;
        aktualnieOdwiedzane = miasto1;
    }
}
void deleteList(struct punkt *listaPunktow){
    struct punkt *tmp;
    while(listaPunktow) {
        tmp = listaPunktow;
        free(listaPunktow);
        listaPunktow = tmp->nastepny;
    }
}
int main(int argc, char **argv) {
    struct punkt *listaPunktow = (struct punkt *)malloc(sizeof(struct punkt)); //utworzenie pierwszego pustego elementu
    struct punkt *listaZachlanny = (struct punkt *)malloc(sizeof(struct punkt));
    double dlugoscZachlanny = 0;

    addPoint(listaPunktow, 195, 273);
    addPoint(listaPunktow, 253, 523);
    addPoint(listaPunktow, 468, 460);
    addPoint(listaPunktow, 459, 276);
    addPoint(listaPunktow, 371, 202);
    addPoint(listaPunktow, 144, 142);
    addPoint(listaPunktow, 37, 228);
    addPoint(listaPunktow, 40, 127);

    printf("Aktualne punkty w bazie:");
    showPoints(listaPunktow);

    algorithmGreedy(listaPunktow, listaZachlanny, &dlugoscZachlanny);

    printf("\nDroga algorytmu zachlannego o trasie: %0.3lf", dlugoscZachlanny);
    showPoints(listaZachlanny);

    deleteList(listaPunktow);
    deleteList(listaZachlanny);
    return 0;
}






