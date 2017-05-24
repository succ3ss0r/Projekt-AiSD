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

struct punkt* znajdzMniejszeX(struct punkt *listaPunktow, int x) {
    //funkcja zwraca adres elementu z mniejszą współrzędną x

    if(!listaPunktow->nastepny || listaPunktow->nastepny->wspX >= x)
        //jeżeli dodawany będzie pierwszy element lub X pierwszego elementu będzie więsze od dodawanego zwróć wskaźnik na listę
        return listaPunktow;

    struct punkt *poprzedni = listaPunktow->nastepny; //utwórz miejsce gdzie będzie przechowany poprzedni element listy
    listaPunktow = listaPunktow->nastepny; //ustaw listę na pierwszy element (nie na wskażnik listy)

    while(listaPunktow->wspX <= x) {
        //dopóki wspX aktualnej listy jest większe bądź równe x oraz istnieją następne elementy w liście

        poprzedni = listaPunktow; //przełącz wskażnik poprzedniego na aktualny element
        if(listaPunktow->nastepny)
            //sprawdza czy istnieje nastepny element w liście
            listaPunktow = listaPunktow->nastepny; //przesuń wskażnik o jeden dalej
        else
            //jeżeli nie ma nastepnego elementu w liście wyjdż z while
            break;
    }

    //zwróć adres element za który trzeba wstawić element
    return poprzedni;
};
void addPoint(struct punkt *listaPunktow, int x, int y) {
    //dodaje tyle punktow ile chce użytkownik
    struct punkt *dodajPo;
        //dodanie tyle punktow ile wybral uzytkownik

        struct punkt *tmp = (struct punkt *) malloc(sizeof(struct punkt)); //zarezerwowanie pamięci

    if(tmp) {
        //jeżeli poprawnie zalokowano pamięć

        //usupełnij współrzedne nowego punktu
        tmp->wspX = x;
        tmp->wspY = y;

        dodajPo = znajdzMniejszeX(listaPunktow, x);

        tmp->nastepny = dodajPo->nastepny;
        dodajPo->nastepny = tmp; // przepnij wskaznik listy na poczatek

        printf("\nDodano punkt (%d, %d)", tmp->wspX, tmp->wspY); //wyświetl komunikat o dodaniu punktu
    } else {
        //nie zalokowano pamięci

        printf("\nNie udalo sie zaalokowac pamieci!");
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

        printf("\nAktualne punkty w bazie: ");
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
int countPoints(struct punkt *listaPunktow) {
    //wyświetlanie elementów listy zwraca numer ostatniego elementu

    int number = 1; //numery porządkowe od 1
    if(!listaPunktow->nastepny)
        //jeżeli lista jest pusta wyświetl ostrzeżenie
        return 0;

    while(listaPunktow->nastepny) {
        //dopkóki element listy wskazuje na następny

        listaPunktow = listaPunktow->nastepny; //pierwszy element jest pusty dlatego trzeba przepiąć go teraz
        ++number; //zwiększ numer porzadkowy
    }

    return number - 1; //zwroc numer porzadkowy ostatniego elementu
}
float calculateDistance(struct punkt *miasto1, struct punkt *miasto2) {
    float X = pow(miasto1->wspX - miasto2->wspX, 2);
    float Y = pow(miasto1->wspY - miasto2->wspY, 2);
    return sqrt(X + Y);
}
void algorithmGreedy(struct punkt *listaPunktow) {
    //algorytm zachlanny
}
int main(int argc, char **argv) {
    struct punkt *listaPunktow = (struct punkt *)malloc(sizeof(struct punkt)); //utworzenie pierwszego pustego elementu

    addPoint(listaPunktow, 5, 4);
    addPoint(listaPunktow, 3, 2);
    addPoint(listaPunktow, 7, 1);
    addPoint(listaPunktow, 1, 8);
    addPoint(listaPunktow, 7, 7);

    showPoints(listaPunktow);

    algorithmGreedy(listaPunktow);

    printf("\nDroga algorytmu zachlannego o trasie: %lf" );

    return 0;
}






