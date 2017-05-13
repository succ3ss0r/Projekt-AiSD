#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/mouse.h>
#include <stdio.h>

#define MARGINES 5

#define SZEROKOSCOKNA 640
#define WYSOKOSCOKNA 600
#define KOLOROKNA al_map_rgb(138,205,255)

#define KOLORPUNKTU al_map_rgba(51,153,51,0.5)
#define KOLOROBRAMOWANIAPUNKTU al_map_rgb(236,249,236)
#define ROZMIARPUNKTU 15

#define WYSOKOSCPRZYCISKOW 50
#define WYSOKOSCPASKAKOLORU 10
#define ZIELONY al_map_rgb(66, 203, 110)
#define ZOLTY al_map_rgb(252, 206, 84)
#define CZERWONY al_map_rgb(237, 85, 100)
#define CZARNY al_map_rgb(0, 0, 0)

#define PASEKSTANU 20

#define MAXELEMENTS 5

struct punkt {
    //struktura potrzebna do stworzenia listy punktow która zostanie przerobiona na tablicę przejść lub listy przejść
    int wspX;
    int wspY;
    int odwiedzony;
    struct punkt *nastepny;
};
struct elementDrogi {
    //struktura przechowuje przejścia pomiędzy poszczególnymi miastami oraz odległości pomiędzy nimi

    struct punkt *miasto;
    struct elementDrogi *nastepneMiasto;
    float dlugoscTrasy;
};
struct drogi {
    //struktura przechowująca listę przejść dla każdego z algorytmów oraz cąłkowitą przebytą trasę

    struct elementDrogi *listaGreedy;
    struct elementDrogi *listaDeterministic;
    struct elementDrogi *listaGenetic;

    float drogaGreedy;
    float drogaDeterministic;
    float drogaGenetic;
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

void clearInside() {
    al_draw_filled_rectangle(0, WYSOKOSCPRZYCISKOW + WYSOKOSCPASKAKOLORU, SZEROKOSCOKNA, WYSOKOSCOKNA - PASEKSTANU, KOLOROKNA);
}

void drawLines(struct punkt *listaPunktow) {
    //rysuje połączenia pomiedzy miejscami na mapie

    struct punkt *copyPunkt = NULL;
    while(listaPunktow->nastepny) {
        listaPunktow=listaPunktow->nastepny;
        copyPunkt = listaPunktow;
        while(copyPunkt){
            al_draw_line(listaPunktow->wspX, listaPunktow->wspY, copyPunkt->wspX, copyPunkt->wspY, CZARNY, 1);
            copyPunkt = copyPunkt->nastepny;
        }
    }
}

void drawLinesToNewPoint(struct punkt *listaPunktow, int x, int y) {

    while(listaPunktow->nastepny) {
        listaPunktow=listaPunktow->nastepny;
            al_draw_line(listaPunktow->wspX, listaPunktow->wspY, x, y, CZARNY, 1);
    }
}

void drawPoints(struct punkt *listaPunktow) {
    drawLines(listaPunktow);

    while( listaPunktow->nastepny ) {
    //przechodzenie po całej liście punktow

        listaPunktow = listaPunktow->nastepny;

        al_draw_filled_circle(listaPunktow->wspX, listaPunktow->wspY, ROZMIARPUNKTU, KOLORPUNKTU);
        al_draw_circle(listaPunktow->wspX, listaPunktow->wspY, ROZMIARPUNKTU, KOLOROBRAMOWANIAPUNKTU, 1);
    }
}


void wyswietlListePuktow(struct punkt *listaPunktow) {
    while(listaPunktow) {
        printf("\nWspolrzedne: %d, %d", listaPunktow->wspX, listaPunktow->wspY);
        listaPunktow = listaPunktow->nastepny;
    }
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

void addPoint(struct punkt *listaPunktow, int x, int y) {
    //dodaje tyle punktow ile chce użytkownik

    if(checkIfExist(x,y, listaPunktow))
        return;

    if(countPoints(listaPunktow) >= MAXELEMENTS) {
        //jeżeli lista jest już pełna

        return;
    }

    struct punkt *dodajPo = NULL;

    struct punkt *tmp = (struct punkt *) malloc(sizeof(struct punkt)); //zarezerwowanie pamięci

    if(tmp) {
        //jeżeli poprawnie zalokowano pamięć

        //usupełnij współrzedne nowego punktu
        tmp->wspX = x;
        tmp->wspY = y;

        dodajPo = znajdzMniejszeX(listaPunktow, x);

        tmp->nastepny = dodajPo->nastepny;
        dodajPo->nastepny = tmp; // przepnij wskaznik listy na poczatek
    }
}


void activateButton(int buttonNo, int *activeButton) {
    ALLEGRO_COLOR color;
    int value = 0;

    if(buttonNo < 640) {
        color = CZARNY;
        value = 3;
    }
    if(buttonNo < 480) {
        color = CZERWONY;
        value = 2;
    }
    if(buttonNo < 320) {
        color = ZOLTY;
        value = 1;
    }
    if(buttonNo < 160) {
        color = ZIELONY;
        value = 0;
    }

    al_draw_filled_rectangle(0, WYSOKOSCPRZYCISKOW, SZEROKOSCOKNA, WYSOKOSCPRZYCISKOW + WYSOKOSCPASKAKOLORU, color);
    *activeButton = value;
}



void drawButtons(int *activeButton, ALLEGRO_FONT *font) {
    //rysuje przyciski

    al_draw_filled_rectangle(0, 0, SZEROKOSCOKNA / 4, WYSOKOSCPRZYCISKOW, ZIELONY);
    al_draw_filled_rectangle(SZEROKOSCOKNA / 4, 0, SZEROKOSCOKNA / 2, WYSOKOSCPRZYCISKOW, ZOLTY);
    al_draw_filled_rectangle(SZEROKOSCOKNA / 2, 0, SZEROKOSCOKNA * 3 / 4, WYSOKOSCPRZYCISKOW, CZERWONY);
    al_draw_filled_rectangle(SZEROKOSCOKNA * 3 / 4, 0, SZEROKOSCOKNA, WYSOKOSCPRZYCISKOW, CZARNY);

    activateButton(0, activeButton);

    al_draw_text(font, al_map_rgb(255,255,255), SZEROKOSCOKNA / 8, WYSOKOSCPRZYCISKOW / 9, ALLEGRO_ALIGN_CENTRE, "DODAJ");
    al_draw_text(font, al_map_rgb(255,255,255), 3 * SZEROKOSCOKNA / 8, WYSOKOSCPRZYCISKOW / 9, ALLEGRO_ALIGN_CENTRE, "EDYTUJ");
    al_draw_text(font, al_map_rgb(255,255,255), 5 * SZEROKOSCOKNA / 8, WYSOKOSCPRZYCISKOW / 9, ALLEGRO_ALIGN_CENTRE, "USUŃ");
    al_draw_text(font, al_map_rgb(255,255,255), 7 * SZEROKOSCOKNA / 8, WYSOKOSCPRZYCISKOW / 9, ALLEGRO_ALIGN_CENTRE, "POLICZ");
}

void changeButton(int x, int y, int *activeButton) {
    if(y < WYSOKOSCPRZYCISKOW) {
        activateButton(x, activeButton);
        al_flip_display();
    }
}

void drawStatusbar() {
    al_draw_filled_rectangle(0, WYSOKOSCOKNA - 10, SZEROKOSCOKNA, WYSOKOSCOKNA, CZARNY);
}

int main(int argc, char **argv) {
    if( !al_init() && !al_init_primitives_addon() ) {
        fprintf(stderr, "Nie mozna zainicjowac biblioteki allegro");
        return -1;
    }
    if( !al_init_font_addon()) {
        fprintf(stderr, "Nie zainicjowano czcionek");
        return -1;
    }
    if( !al_init_ttf_addon() ) {
        fprintf(stderr, "Nie zainicjowano czcionek");
        return -1;
    }
    if( !al_install_mouse() ) {
        fprintf(stderr, "Nie udalo sie zainicjowac myszy");
        return -1;
    }
    if( !al_init_primitives_addon() ) {
        fprintf(stderr, "Nie udalo sie zainicjowac primitywow");
    }



    struct punkt *listaPunktow = (struct punkt *)malloc(sizeof(struct punkt));


    ALLEGRO_DISPLAY *oknoKomiwojazera = al_create_display(SZEROKOSCOKNA, WYSOKOSCOKNA);
    if( !oknoKomiwojazera ) {
        fprintf(stderr, "Nie mozna zainicjowac okna");
        return -1;
    }
    ALLEGRO_FONT *font72 = al_load_ttf_font("font.ttf", 30, 0);
    if( !font72 ){
        fprintf(stderr, "Nie udalo sie zainicjowac czcionki");
        return -1;
    }
    ALLEGRO_EVENT_QUEUE *kolejkaZdarzen = al_create_event_queue();
    if( !kolejkaZdarzen ) {
        fprintf(stderr, "Nie mozna utworzyc kolejki zdarzen");
        return -1;
    }

    al_register_event_source(kolejkaZdarzen, al_get_display_event_source(oknoKomiwojazera));
    al_register_event_source(kolejkaZdarzen, al_get_mouse_event_source());

    al_clear_to_color(KOLOROKNA);
    int activeButton;
    drawButtons(&activeButton, font72);

    al_flip_display();

    ALLEGRO_MOUSE_STATE wlasciwoscMyszy;
    bool polozenieMyszyWewnatrzPolaRysowania = NULL;


    while(1) {

        ALLEGRO_EVENT ev;
        ALLEGRO_TIMEOUT timeout;
        al_init_timeout(&timeout, 0.06);

        bool get_event = al_wait_for_event_until(kolejkaZdarzen, &ev, &timeout);

        if(get_event && ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
        //jeżeli zamkniecie okna przez X na pasku okna
            break; //wyjdź z pętli while
        }

        al_get_mouse_state(&wlasciwoscMyszy); //pobieraj informacje o myszy
        if(activeButton == 0) {
        //jeżeli jest aktywny tryb dodawania punktu

            polozenieMyszyWewnatrzPolaRysowania = wlasciwoscMyszy.y > WYSOKOSCPRZYCISKOW + WYSOKOSCPASKAKOLORU + ROZMIARPUNKTU && wlasciwoscMyszy.y < WYSOKOSCOKNA - PASEKSTANU - ROZMIARPUNKTU && wlasciwoscMyszy.x > ROZMIARPUNKTU && wlasciwoscMyszy.x < SZEROKOSCOKNA - ROZMIARPUNKTU;
            if(polozenieMyszyWewnatrzPolaRysowania) {
            //jeżeli chcesz machać samym punktem
                if(countPoints(listaPunktow) == MAXELEMENTS) {
                //jeżeli jest już maksymalna liczba elementów
                    al_set_system_mouse_cursor(oknoKomiwojazera, ALLEGRO_SYSTEM_MOUSE_CURSOR_UNAVAILABLE);
                    al_show_mouse_cursor(oknoKomiwojazera);
                    continue;
                }
                if(get_event && ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN){
                //jeżeli kliknięcie
                    clearInside();
                    addPoint(listaPunktow, wlasciwoscMyszy.x, wlasciwoscMyszy.y);
                    drawPoints(listaPunktow);
                    al_flip_display();
                } else {
                //jeżeli tylko machanie myszką
                    al_hide_mouse_cursor(oknoKomiwojazera);
                    clearInside();
                    drawLinesToNewPoint(listaPunktow, wlasciwoscMyszy.x, wlasciwoscMyszy.y);
                    drawPoints(listaPunktow);
                    al_draw_filled_circle(wlasciwoscMyszy.x, wlasciwoscMyszy.y, ROZMIARPUNKTU, KOLORPUNKTU);
                    al_draw_circle(wlasciwoscMyszy.x, wlasciwoscMyszy.y, ROZMIARPUNKTU, KOLOROBRAMOWANIAPUNKTU, 1);
                    al_flip_display();
                }
            } else {
            //jeżeli mysz nie znajduje się w polu wyznaczonym do rysowania
                al_show_mouse_cursor(oknoKomiwojazera);
                if(get_event && ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
                //jeżeli nastąpi kliknięcie
                    changeButton(wlasciwoscMyszy.x, wlasciwoscMyszy.y, &activeButton); //zmień przycisk myszy
                } else {
                    clearInside();
                    drawPoints(listaPunktow);
                    al_flip_display();
                }
            }
        }
        if(activeButton == 1) {
        //jeżeli jest aktywny tryb modyfikacji punktow

            if(get_event && ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            //jeżeli został wciśnięty przycisk myszy

                changeButton(wlasciwoscMyszy.x, wlasciwoscMyszy.y, &activeButton);
            }
        }
        if(activeButton == 2) {
        //jeżeli jest aktywny tryb usuwania punktow

            if(get_event && ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            //jeżeli został wciśnięty przycisk myszy
                changeButton(wlasciwoscMyszy.x, wlasciwoscMyszy.y, &activeButton);
            }
        }
        if(activeButton == 3) {
        //jeżeli jest aktywny tryb usuwania punktow
            if(get_event && ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            //jeżeli został wciśnięty przycisk myszy
                changeButton(wlasciwoscMyszy.x, wlasciwoscMyszy.y, &activeButton);
            }
        }
        al_rest(0.001);
    }

    al_destroy_display(oknoKomiwojazera);
    al_destroy_event_queue(kolejkaZdarzen);

    return 0;
}
