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
int checkIfExist(int x, int y, struct punkt *listaPunktow) {
    //sprawdza czy istnieje miasto o danych współrzędnych w liście

    while(listaPunktow->nastepny) {
        //dopkóki element listy wskazuje na następny

        listaPunktow = listaPunktow->nastepny; //pierwszy element jest pusty dlatego trzeba przepiąć go teraz
        if( listaPunktow->wspX == x && listaPunktow->wspY == y)
            return 1;
    }

    return 0;
}
float calculateDistance(struct punkt *miasto1, struct punkt *miasto2) {
    float X = pow(miasto1->wspX - miasto2->wspX, 2);
    float Y = pow(miasto1->wspY - miasto2->wspY, 2);
    return sqrt(X + Y);
}
bool czyOdwiedzonoWszystkie(struct punkt *listaPunktow) {
    while(listaPunktow){
        if(listaPunktow->odwiedzony == false)
            return true;
        listaPunktow = listaPunktow->nastepny;
    }
    return false;
}
struct punkt *wolneMiasto(struct punkt *listaPunktow) {
    while(listaPunktow) {
        if(listaPunktow->odwiedzony == false)
            return listaPunktow;
        listaPunktow = listaPunktow->nastepny;
    }
    return NULL;
}
void algorithmGreedy(struct punkt *listaPunktow, struct punkt *listaZachlanny, double *dlugoscZachlanny) {
    //algorytm zachlanny
    listaPunktow = listaPunktow->nastepny; //przepiecie na pierwszy element, zeby nie wskazywal na wartownika

    struct punkt *cp_listaPunktow = listaPunktow; //zapamietanie wskaznika na liste
    addPoint(listaZachlanny, listaPunktow->wspX, listaPunktow->wspY); //dodanie pierwszego punktu
    listaPunktow->odwiedzony = true;

    struct punkt *miastoOdwiedzane = listaPunktow; //miasto w ktorym aktualnie znajduje sie komiwojazer
    struct punkt *miastoProba = NULL; //miasto do ktorego planuje isc
    struct punkt *miastoTMP = NULL; //miasto do ktorego planuje isc

    double odlegloscProba, odlegloscTMP;

    while(czyOdwiedzonoWszystkie(cp_listaPunktow)) {
    //dopóki jest jakieś miasto nieodwiedzone
        listaPunktow = cp_listaPunktow; //ustaw wskaźnik na początek listy
        miastoProba = wolneMiasto(cp_listaPunktow->nastepny); //znajdź I wolne miasto
        miastoProba->odwiedzony = true;
        while(listaPunktow) {
        //przejdź przez całą listę
            miastoTMP =  wolneMiasto(miastoProba); //znajdz II wolne miasto w liscie
            if( !miastoTMP)
            //jeżeli funkcja wolneMiasto zwróciła NULL do któregoś z miast
                break; //przerwij pętle
            odlegloscProba = calculateDistance(miastoOdwiedzane, miastoProba);
            odlegloscTMP = calculateDistance(miastoOdwiedzane, miastoTMP);
            if(odlegloscProba > odlegloscTMP) {
            //jeżeli odległość do I wolnego miasta jest większa niż do drugiego wolnego
                miastoProba->odwiedzony = false;
                miastoProba = miastoTMP;
            }
            listaPunktow = listaPunktow->nastepny;
        }
        addPoint(listaZachlanny, miastoProba->wspX, miastoProba->wspY);
        miastoProba->odwiedzony = true;
        miastoOdwiedzane = miastoProba;
    }
}
int main(int argc, char **argv) {
    struct punkt *listaPunktow = (struct punkt *)malloc(sizeof(struct punkt)); //utworzenie pierwszego pustego elementu
    struct punkt *listaZachlanny = (struct punkt *)malloc(sizeof(struct punkt));
    double dlugoscZachlanny = 0;

    addPoint(listaPunktow, 1, 8);
    addPoint(listaPunktow, 3, 2);
    addPoint(listaPunktow, 5, 4);
    addPoint(listaPunktow, 7, 1);
    addPoint(listaPunktow, 7, 7);

    printf("Aktualne punkty w bazie:");
    showPoints(listaPunktow);

    algorithmGreedy(listaPunktow, listaZachlanny, &dlugoscZachlanny);

    printf("\nDroga algorytmu zachlannego o trasie: %0.3lf", dlugoscZachlanny);
    showPoints(listaZachlanny);

    return 0;
}






