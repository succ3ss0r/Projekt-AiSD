#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAXELEMENTS 30

struct punkt {
    int wspX;
    int wspY;
    int odwiedzony;
    struct punkt *nastepny;
};

void menu();
int choose(int, int);
int showPoints(const struct punkt *);
int checkIfExist(int, int, struct punkt *);
void addPoint(struct punkt *);
void removePoint(struct punkt *);

int countPoints(const struct punkt *listaPunktow) {
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



int main(int argc, char **argv) {
    struct punkt *listaPunktow = (struct punkt *) malloc(sizeof(struct punkt)); //utworzenie pierwszego pustego elementu
    srand(time(NULL)); //ziarno do losowania punktów

    int opcja = 1; //strażnik pętli

    while(opcja) {
        //dopóki zmienna opcja ma wartość różną od 0

        menu(); //wyświetl menu
        opcja = choose(0, 4); //wybór z walidacją

        if(!opcja) {
            //jeżeli opcja będzie zerem pożegnaj i wyjdź

            printf("\nDziekuje za skorzystanie z programu");
            return 0;
        }

        if(opcja == 1) {
            //jeżeli opcja będzie równa 1 dodaj punkt

            addPoint(listaPunktow);
        }
        if(opcja == 2) {
            //jeżeli opcja będzie równa 2 usuń punkt

            removePoint(listaPunktow);
        }
        if(opcja == 3) {
            //jeżeli opcja bedzie równa 3 wyświetl wszystkie punkty

            showPoints(listaPunktow);
        }

        printf("\n\n");
    }

}



void menu() {
    //wyswietla menu wyboru

    printf("Wybierz jedna z ponizszych opcji:\n");
    printf("\t0. Zakonczenie\n");
    printf("\t1. Dodanie punktu\n");
    printf("\t2. Usuniecie punktu\n");
    printf("\t3. Wyswietlenie wszystkich punktow\n");
    printf("\t4. Obliczenie trasy\n");
}

int choose(int min, int max) {
    //wybór opcji wraz z walidacją

    while(1) {
        int opcjaTmp = 0;

        printf("\nWybierz z zakresu %d - %d: ", min, max);
        if(scanf("%d", &opcjaTmp) == 1 && opcjaTmp >= min && opcjaTmp <= max) {
            return opcjaTmp;
        } else {
            printf("\n\nWybor nie jest poprawny!");
            return -1;
        }
    }
}

int showPoints(const struct punkt *listaPunktow) {
    //wyświetlanie elementów listy zwraca numer ostatniego elementu

    printf("\nAktualne punkty w bazie: ");

    int number = 1; //numery porządkowe od 1
    if(!listaPunktow->nastepny)
        //jeżeli lista jest pusta wyświetl ostrzeżenie
        printf("\n\tBrak punktow! Najpierw dodaj ich kilka.");

    while(listaPunktow->nastepny) {
        //dopkóki element listy wskazuje na następny

        listaPunktow = listaPunktow->nastepny; //pierwszy element jest pusty dlatego trzeba przepiąć go teraz
        printf("\n\t%d. (%d, %d)", number, listaPunktow->wspX, listaPunktow->wspY);

        ++number; //zwiększ numer porzadkowy
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

void addPoint(struct punkt *listaPunktow) {
    //dodaje tyle punktow ile chce użytkownik


    if(countPoints(listaPunktow) >= MAXELEMENTS) {
        //jeżeli lista jest już pełna

        printf("\nOsiagnieto maksymalna liczbe %d elementow. Nie mozna dodac wiecej.", MAXELEMENTS);
        return;
    }

    printf("\nPodaj ile punktów chcesz dodac");
    int opcja = choose(0, 30 - countPoints(listaPunktow)), i = 0;
    while(i < opcja) {
        //dodanie tyle punktow ile wybral uzytkownik

        struct punkt *tmp = (struct punkt *) malloc(sizeof(struct punkt)); //zarezerwowanie pamięci

        if(tmp) {
            //jeżeli poprawnie zalokowano pamięć

            int x, y; //tymczasowe wspolrzedne które będą porównywane z wartosciami w liscie
            do {
                //losowanie wartosci dopóki nie bedą się powtarzać w liście

                x = 10 + rand() % 610; //640 oraz pozostawione 10px od krańców ekranu (przestrzeń robocza 620px)
                y = 10 + rand() % 450; //480 oraz pozostawione 10px od krańców ekranu (przestrzeń robocza 460px)
            } while(checkIfExist(x, y, listaPunktow));

            //usupełnij współrzedne nowego punktu

            tmp->wspX = x;
            tmp->wspY = y;

            tmp->nastepny = listaPunktow->nastepny; //ustaw wskaznik nowego elementu na stary listy
            listaPunktow->nastepny = tmp; // przepnij wskaznik listy na poczatek

            printf("\nDodano punkt (%d, %d)", tmp->wspX, tmp->wspY); //wyświetl komunikat o dodaniu punktu
            ++i;
        } else {
            //nie zalokowano pamięci
            printf("\nNie udalo sie zaalokowac pamieci!");
            return;
        }
    }
    if(opcja == 0)
        //jeżeli chcemy dodać 0 punktów
        printf("\nNie dodano zadnego punktu.");
    else
        //jeżeli dodało się kilka punktów
        printf("\nDodano %d punktow.", i);

}


void removePoint(struct punkt *listaPunktow) {
    //usuwanie wskazanego punktu

    int opcja = -1, max = showPoints(listaPunktow); //wyświetlenie wszystkich punktow oraz zapisanie ostatniego numeru porzadkowego do zmiennej

    printf("\nWybierz punkt do usunięcia");

    do {
        opcja = choose(1, max);
    } while(opcja == -1);


    struct punkt *poprzedni = NULL; //struktura na potrzeby zachowania adresu elementu poprzedzajacego element, który chcemy usunąć
    for(int i = 0; i < opcja; ++i) {
        poprzedni = listaPunktow;
        listaPunktow = listaPunktow->nastepny; //wykorzystanie przekazanej kopii wskaznika na liste
    }

    poprzedni->nastepny = listaPunktow->nastepny; //przepiecie wskaznika poprzedniego elementu, żeby wskazywał na ten sam element na ktory wskazuje jego nastepca

    free(listaPunktow); //zwolnienie pamieci
}
