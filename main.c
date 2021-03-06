/****************************************************************************
*                          Problem komiwojażera                             *
*                      Paweł Siwoń, Jakub Serwicki                          *
*           Wydział Elektrotechniki, Automatyki i Informatyki               *
*                        Grupa dziekańska 1ID16A                            *
*                                                                           *
*            Wszelkie prawa do wykorzystania kodu zastrzeżone               *
*                    Politechnika Świętokrzyska 2017                        *
****************************************************************************/

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/mouse.h>
#include <stdio.h>
#include <math.h>
#define MARGINES 0

#define SZEROKOSCOKNA 640
#define WYSOKOSCOKNA 600
#define KOLOROKNA al_map_rgb(138,205,255)

#define KOLORPUNKTU al_map_rgba(0,24,114,150)
#define KOLOROBRAMOWANIAPUNKTU al_map_rgb(236,249,236)
#define KOLORLINI al_map_rgb(0,178,255)
#define KOLORLININOWYPUNKT al_map_rgba(51,153,51,0.5)
#define ROZMIARPUNKTU 15
#define ROZMIARCZCIONKIPRZYCISK 30
#define ROZMIARCZCIONKIPUNKT 15

#define WYSOKOSCPRZYCISKOW 50
#define WYSOKOSCPASKAKOLORU 10
#define ZIELONY al_map_rgb(66, 203, 110)
#define ZOLTY al_map_rgb(252, 206, 84)
#define CZERWONY al_map_rgb(237, 85, 100)
#define CZARNY al_map_rgb(0, 0, 0)

#define PASEKSTANU 20

#define MAXELEMENTS 21

struct punkt {
    int wspX, wspY;
    bool odwiedzony;
    struct punkt *nastepny;
};

void activateButton(int buttonNo, int *activeButton);
void addPoint(struct punkt *listaPunktow, int x, int y);
void addModifiedPoint(struct punkt *listaPunktow, int x, int y);
void algorithmGreedy(struct punkt *listaPunktow, struct punkt *listaZachlanny);
int allegroInitializeAllAddons(void);
double calculateDistance(struct punkt *miasto1, struct punkt *miasto2);
double countPath(struct punkt *listaPunktow);
int countPoints(struct punkt *listaPunktow);
void changeButton(int x, int y, int *activeButton);
int checkIfExist(int x, int y, struct punkt *listaPunktow);
void clearInside(void);
void deleteList(struct punkt *listaPunktow);
void deletePoint(struct punkt *prev);
void drawButtons(int *activeButton, ALLEGRO_FONT *font);
void drawLines(struct punkt *listaPunktow);
void drawLinesToNewPoint(struct punkt *listaPunktow, int x, int y);
void drawPoints(struct punkt *listaPunktow);
void drawPointsWithoutLines(struct punkt *listaPunktow);
void drawStatusbar(void);
void printHowLong(double trasa, ALLEGRO_FONT *circleFont);
void showPointsPath(struct punkt *listaPunktow, ALLEGRO_FONT *circleFont);
struct punkt *takePoint(struct punkt *listaPunktow, int x, int y);
void unvisitAll(struct punkt *listaPunktow);

int main(int argc, char **argv) {
    if( allegroInitializeAllAddons() ) {
        return -1;
    }

    //inicjalizacja potrzebnych struktur
    struct punkt *listaPunktow = (struct punkt *)calloc(1, sizeof(struct punkt)); //utworzenie pierwszego pustego elementu
    struct punkt *listaZachlanny = (struct punkt *)calloc(1, sizeof(struct punkt));

    ALLEGRO_DISPLAY *oknoKomiwojazera = al_create_display(SZEROKOSCOKNA, WYSOKOSCOKNA);
    ALLEGRO_FONT *font72 = al_load_ttf_font("font.ttf", ROZMIARCZCIONKIPRZYCISK, 0);
    ALLEGRO_FONT *circleFont = al_load_ttf_font("font.ttf", ROZMIARCZCIONKIPUNKT, 0);
    ALLEGRO_EVENT_QUEUE *kolejkaZdarzen = al_create_event_queue();
    if( !listaPunktow ) {
        fprintf(stderr, "Nie udalo sie zainicjowac listy punktow");
        return -1;
    }
    if( !oknoKomiwojazera ) {
        fprintf(stderr, "Nie mozna zainicjowac okna");
        return -1;
    }
    if( !font72 ){
        fprintf(stderr, "Nie udalo sie zainicjowac czcionki");
        return -1;
    }
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
    bool modyfikacjaPunktu = false;
    int tmpX, tmpY;
    tmpX = tmpY = 0;
    bool busyMouse = false;
    bool policzonaSciezka = false;
    double dlugoscTrasy = 0;
    struct punkt *elementListy = NULL;

    printHowLong(0, circleFont);

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
                    policzonaSciezka = false; //oznaczenie ze potrzeba policzyc sciezke od nowa
                    unvisitAll(listaPunktow); //oznaczenie miast jako nieodwiedzonych
                    deleteList(listaZachlanny); //oproznienie trasy
                    printHowLong(0, circleFont); // usuniecie napisu

                    clearInside();
                    addPoint(listaPunktow, wlasciwoscMyszy.x, wlasciwoscMyszy.y);
                    drawPoints(listaPunktow);
                    al_flip_display();
                } else {
                //jeżeli tylko machanie myszką
                    al_set_system_mouse_cursor(oknoKomiwojazera, ALLEGRO_SYSTEM_MOUSE_CURSOR_PRECISION);
                    clearInside();
                    drawLinesToNewPoint(listaPunktow, wlasciwoscMyszy.x, wlasciwoscMyszy.y);
                    drawPoints(listaPunktow);
                    al_draw_filled_circle(wlasciwoscMyszy.x, wlasciwoscMyszy.y, ROZMIARPUNKTU, KOLORPUNKTU);
                    al_draw_circle(wlasciwoscMyszy.x, wlasciwoscMyszy.y, ROZMIARPUNKTU, KOLOROBRAMOWANIAPUNKTU, 1);
                    al_flip_display();
                }
            } else {
            //jeżeli mysz nie znajduje się w polu wyznaczonym do rysowania
                if(get_event && ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
                //jeżeli nastąpi kliknięcie
                    changeButton(wlasciwoscMyszy.x, wlasciwoscMyszy.y, &activeButton); //zmień przycisk myszy
                } else {
                    if(wlasciwoscMyszy.y < WYSOKOSCPRZYCISKOW)
                        al_set_system_mouse_cursor(oknoKomiwojazera, ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK);
                    else
                        al_set_system_mouse_cursor(oknoKomiwojazera, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
                    al_show_mouse_cursor(oknoKomiwojazera);
                    clearInside();
                    drawPoints(listaPunktow);
                    al_flip_display();
                }
            }
        }
        if(activeButton == 1) {
        //jeżeli jest aktywny tryb modyfikacji punktów
            polozenieMyszyWewnatrzPolaRysowania = wlasciwoscMyszy.y > WYSOKOSCPRZYCISKOW + WYSOKOSCPASKAKOLORU + ROZMIARPUNKTU && wlasciwoscMyszy.y < WYSOKOSCOKNA - PASEKSTANU - ROZMIARPUNKTU && wlasciwoscMyszy.x > ROZMIARPUNKTU && wlasciwoscMyszy.x < SZEROKOSCOKNA - ROZMIARPUNKTU;
            if(polozenieMyszyWewnatrzPolaRysowania) {
            //jeżeli znajduje sie w polu rysowania
                if(modyfikacjaPunktu == false ) {
                //jeżeli nie jest włączona edycja punktu
                    elementListy = takePoint(listaPunktow, wlasciwoscMyszy.x, wlasciwoscMyszy.y);
                    if(elementListy) {
                    //jeżeli zwróciło mi element na którym jest myszka
                        al_set_system_mouse_cursor(oknoKomiwojazera, ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK);
                    }
                    else {
                        al_set_system_mouse_cursor(oknoKomiwojazera, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
                        tmpX = tmpY = 0;
                    }
                    if(get_event && ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && elementListy) {
                    //jeżeli zwróciło element i został on wciśnięty
                        policzonaSciezka = false; //oznaczenie ze potrzeba policzyc sciezke od nowa
                        unvisitAll(listaPunktow); //oznaczenie miast jako nieodwiedzonych
                        deleteList(listaZachlanny); //oproznienie trasy
                        printHowLong(0, circleFont); //usuniecie trasy ze spodu ekranu


                        al_show_mouse_cursor(oknoKomiwojazera);
                        tmpX = elementListy->nastepny->wspX, tmpY = elementListy->nastepny->wspY;
                        deletePoint(elementListy);

                        clearInside();
                        modyfikacjaPunktu = true;
                        continue;
                    }
                    clearInside();
                    drawPoints(listaPunktow);
                    al_flip_display();
                    al_show_mouse_cursor(oknoKomiwojazera);
                } else {
                //jezeli jestem w trakcie modyfikacji punktu
                    if(get_event && ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
                    //jezeli klikne aby nadac nowa pozycje punktowi
                        addModifiedPoint(elementListy, wlasciwoscMyszy.x, wlasciwoscMyszy.y);
                        elementListy = NULL;
                        modyfikacjaPunktu = false;
                        continue;
                    } else {
                        al_set_system_mouse_cursor(oknoKomiwojazera, ALLEGRO_SYSTEM_MOUSE_CURSOR_MOVE);
                        clearInside();
                        drawLines(listaPunktow);
                        drawLinesToNewPoint(listaPunktow, wlasciwoscMyszy.x, wlasciwoscMyszy.y);
                        drawPointsWithoutLines(listaPunktow);
                        al_draw_filled_circle(wlasciwoscMyszy.x, wlasciwoscMyszy.y, ROZMIARPUNKTU, KOLORPUNKTU);
                        al_draw_circle(wlasciwoscMyszy.x, wlasciwoscMyszy.y, ROZMIARPUNKTU, KOLOROBRAMOWANIAPUNKTU, 1);
                        al_flip_display();
                    }
                }
            } else {
            //jeżeli mysz nie znajduje się w polu wyznaczonym do rysowania
                if(get_event && ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
                //jeżeli nastąpi kliknięcie
                    changeButton(wlasciwoscMyszy.x, wlasciwoscMyszy.y, &activeButton); //zmień przycisk myszy
                    if(modyfikacjaPunktu) {

                        addModifiedPoint(elementListy, tmpX, tmpY);
                        tmpX = tmpY = 0;
                        modyfikacjaPunktu = false;
                    }
                } else {
                    if(wlasciwoscMyszy.y < WYSOKOSCPRZYCISKOW)
                        al_set_system_mouse_cursor(oknoKomiwojazera, ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK);
                    else
                        al_set_system_mouse_cursor(oknoKomiwojazera, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
                    al_show_mouse_cursor(oknoKomiwojazera);
                    clearInside();
                    drawPoints(listaPunktow);
                    al_flip_display();
                }
            }
        }
        if(activeButton == 2) {
        //jeżeli jest aktywny tryb usuwania punktow
            polozenieMyszyWewnatrzPolaRysowania = wlasciwoscMyszy.y > WYSOKOSCPRZYCISKOW + WYSOKOSCPASKAKOLORU + ROZMIARPUNKTU && wlasciwoscMyszy.y < WYSOKOSCOKNA - PASEKSTANU - ROZMIARPUNKTU && wlasciwoscMyszy.x > ROZMIARPUNKTU && wlasciwoscMyszy.x < SZEROKOSCOKNA - ROZMIARPUNKTU;
            elementListy = takePoint(listaPunktow, wlasciwoscMyszy.x, wlasciwoscMyszy.y);
            if(polozenieMyszyWewnatrzPolaRysowania) {
                if(elementListy)
                    al_set_system_mouse_cursor(oknoKomiwojazera, ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK);
                else
                    al_set_system_mouse_cursor(oknoKomiwojazera, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
                al_show_mouse_cursor(oknoKomiwojazera);
                if(get_event && ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && elementListy) {
                    //jezeli usuwa przycisk
                    policzonaSciezka = false; //oznaczenie ze potrzeba policzyc sciezke od nowa
                    unvisitAll(listaPunktow); //oznaczenie miast jako nieodwiedzonych
                    deleteList(listaZachlanny); //oproznienie trasy
                    printHowLong(0, circleFont); //usuniecie trasy ze spodu ekranu

                    deletePoint(elementListy);
                    clearInside();
                    drawPoints(listaPunktow);
                    al_flip_display();
                    continue;
                }
            } else {
            //jeżeli mysz nie znajduje się w polu wyznaczonym do rysowania
                if(get_event && ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
                //jeżeli nastąpi kliknięcie
                    changeButton(wlasciwoscMyszy.x, wlasciwoscMyszy.y, &activeButton); //zmień przycisk myszy
                    modyfikacjaPunktu = false;
                } else {
                    if(wlasciwoscMyszy.y < WYSOKOSCPRZYCISKOW)
                        al_set_system_mouse_cursor(oknoKomiwojazera, ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK);
                    else
                        al_set_system_mouse_cursor(oknoKomiwojazera, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
                    al_show_mouse_cursor(oknoKomiwojazera);
                    clearInside();
                    drawPoints(listaPunktow);
                    al_flip_display();
                }
            }
        }
        if(activeButton == 3) {
        //jeżeli jest aktywny tryb liczenia trasy
            if(!busyMouse && !policzonaSciezka) {
                al_set_system_mouse_cursor(oknoKomiwojazera, ALLEGRO_SYSTEM_MOUSE_CURSOR_BUSY);
                busyMouse = true;
            }
            if(wlasciwoscMyszy.y < WYSOKOSCPRZYCISKOW && wlasciwoscMyszy.x < SZEROKOSCOKNA * 3 / 4) {
                al_set_system_mouse_cursor(oknoKomiwojazera, ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK);
                busyMouse = false;
            } else if(!busyMouse) {
                al_set_system_mouse_cursor(oknoKomiwojazera, ALLEGRO_SYSTEM_MOUSE_CURSOR_BUSY);
                busyMouse = true;
            }
            if(get_event && ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && wlasciwoscMyszy.y < WYSOKOSCPRZYCISKOW && wlasciwoscMyszy.x < SZEROKOSCOKNA * 3 / 4) {
            //jeżeli został wciśnięty przycisk myszy
                changeButton(wlasciwoscMyszy.x, wlasciwoscMyszy.y, &activeButton);
                busyMouse = false;
//tutaj usuwanie powinno sie odbyc lub wyczyszczenie policzonej listy
            }
            if( policzonaSciezka == true ) {
            //jeżeli ścieżka była już policzona wyświetlą ją
                showPointsPath(listaZachlanny, circleFont);
                printHowLong(dlugoscTrasy, circleFont);
                al_set_system_mouse_cursor(oknoKomiwojazera, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
                al_flip_display();
            }
            if( policzonaSciezka == false ) {
            //policz ścieżkę
                algorithmGreedy(listaPunktow, listaZachlanny);
                dlugoscTrasy = countPath(listaZachlanny);
                policzonaSciezka = true;
            }


        }
        al_rest(0.0025);
    }

    deleteList(listaPunktow);
    deleteList(listaZachlanny);

    al_destroy_display(oknoKomiwojazera);
    al_destroy_event_queue(kolejkaZdarzen);
    return 0;
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
            al_draw_line(listaPunktow->wspX, listaPunktow->wspY, copyPunkt->wspX, copyPunkt->wspY, KOLORLINI, 1);
            copyPunkt = copyPunkt->nastepny;
        }
    }
}
void drawLinesToNewPoint(struct punkt *listaPunktow, int x, int y) {

    while(listaPunktow->nastepny) {
        listaPunktow=listaPunktow->nastepny;
            al_draw_line(listaPunktow->wspX, listaPunktow->wspY, x, y, KOLORLININOWYPUNKT, 1);
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
void drawPointsWithoutLines(struct punkt *listaPunktow) {
    while( listaPunktow->nastepny ) {
    //przechodzenie po całej liście punktow

        listaPunktow = listaPunktow->nastepny;

        al_draw_filled_circle(listaPunktow->wspX, listaPunktow->wspY, ROZMIARPUNKTU, KOLORPUNKTU);
        al_draw_circle(listaPunktow->wspX, listaPunktow->wspY, ROZMIARPUNKTU, KOLOROBRAMOWANIAPUNKTU, 1);
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

    if(checkIfExist(x, y, listaPunktow))
        return;
    //dodaje na koniec listy
    while(listaPunktow->nastepny)
        listaPunktow = listaPunktow->nastepny;

    struct punkt *tmp = (struct punkt *)calloc(1, sizeof(struct punkt)); //zarezerwowanie pamięci

    if(tmp) {
        //jeżeli poprawnie zalokowano pamięć

        //usupełnij współrzedne nowego punktu
        tmp->wspX = x;
        tmp->wspY = y;
        tmp->nastepny = NULL;
        listaPunktow->nastepny = tmp; // przepnij wskaznik listy na poczatek
    } else {
        //nie zalokowano pamięci
        fprintf(stderr, "\nNie udalo sie zaalokowac pamieci!");
        return;
    }
}
void addModifiedPoint(struct punkt *listaPunktow, int x, int y) {
    //dodaje na poczatek listy punkt
    struct punkt *tmp = (struct punkt *)calloc(1, sizeof(struct punkt)); //zarezerwowanie pamięci
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
int allegroInitializeAllAddons(){
    if( !al_init() ) {
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
    return 0;
}
struct punkt *takePoint(struct punkt *listaPunktow, int x, int y) {
    //zwraca wskaznik na punkt który chcemy przenieść
    int odleglosc = 0;
    struct punkt *prev = NULL;
    while(listaPunktow->nastepny){
        prev = listaPunktow;
        listaPunktow = listaPunktow->nastepny;
        odleglosc = sqrt( pow(listaPunktow->wspX - x,2) + pow(listaPunktow->wspY - y,2) );
        if( odleglosc < ROZMIARPUNKTU)
            return prev;
    }
    return NULL;
}
void deletePoint(struct punkt *prev) {
    //uswuwa punkt nastepny w liscie
    struct punkt *tmp = prev->nastepny;
    prev->nastepny = tmp->nastepny;
    free(tmp);
}
double calculateDistance(struct punkt *miasto1, struct punkt *miasto2) {
    if(miasto2 == NULL)
        return pow(miasto1->wspX, 2);
    double X = pow(miasto1->wspX - miasto2->wspX, 2);
    double Y = pow(miasto1->wspY - miasto2->wspY, 2);
    return sqrt(X + Y);
}
void algorithmGreedy(struct punkt *listaPunktow, struct punkt *listaZachlanny) {
    //algorytm zachlanny
    if(listaPunktow->nastepny == NULL)
        return;
    struct punkt *aktualnieOdwiedzane = listaPunktow->nastepny;
    aktualnieOdwiedzane->odwiedzony = true;
    addPoint(listaZachlanny, aktualnieOdwiedzane->wspX, aktualnieOdwiedzane->wspY);
    struct punkt *miasto1, *miasto2;
    listaPunktow = listaPunktow->nastepny;
    struct punkt *listaPunktow_CP = aktualnieOdwiedzane;
    struct punkt *poczatekListy = aktualnieOdwiedzane;
    while(listaPunktow_CP->nastepny) {
        listaPunktow_CP = listaPunktow_CP->nastepny;
        miasto1 = miasto2 = NULL;
        listaPunktow = poczatekListy;
        while(listaPunktow){
            if(listaPunktow->odwiedzony) {
                listaPunktow = listaPunktow->nastepny;
                continue;
            }
            if(miasto1 == NULL) {
                miasto1 = listaPunktow;
                listaPunktow = listaPunktow->nastepny;
                continue;
            }
            if(miasto2 == NULL) {
                miasto2 = listaPunktow;
                listaPunktow = listaPunktow->nastepny;
                continue;
            }
            if(miasto1 == listaPunktow || miasto2 == listaPunktow) {
                listaPunktow = listaPunktow->nastepny;
                continue;
            }
            if(calculateDistance(aktualnieOdwiedzane, miasto1) > calculateDistance(aktualnieOdwiedzane, miasto2)) {
                miasto1 = miasto2;
                miasto2 = NULL;
            }
            miasto2 = listaPunktow;
            listaPunktow = listaPunktow->nastepny;
        }
        if(calculateDistance(aktualnieOdwiedzane, miasto1) > calculateDistance(aktualnieOdwiedzane, miasto2)) {
            miasto1 = miasto2;
            miasto2 = NULL;
        }
        addPoint(listaZachlanny, miasto1->wspX, miasto1->wspY);
        miasto1->odwiedzony = true;
        aktualnieOdwiedzane = miasto1;
    }
}
void unvisitAll(struct punkt *listaPunktow){
    while(listaPunktow){
        listaPunktow->odwiedzony = false;
        listaPunktow = listaPunktow->nastepny;
    }
}
void deleteList(struct punkt *listaPunktow){
    struct punkt *tmp;
    while(listaPunktow) {
        tmp = listaPunktow->nastepny;
        listaPunktow->nastepny = NULL;
        free(listaPunktow->nastepny);
        listaPunktow = tmp;
    }
}
void showPointsPath(struct punkt *listaPunktow, ALLEGRO_FONT *circleFont) {
    clearInside();
    listaPunktow = listaPunktow->nastepny;
    struct punkt *pierwszy = listaPunktow;
    int number = 1;
    if(!listaPunktow)
        return;
    while(listaPunktow->nastepny) {
        al_draw_line(listaPunktow->wspX, listaPunktow->wspY, listaPunktow->nastepny->wspX, listaPunktow->nastepny->wspY, KOLORLINI, 2);
        listaPunktow = listaPunktow->nastepny;
    }
    al_draw_line(pierwszy->wspX, pierwszy->wspY, listaPunktow->wspX, listaPunktow->wspY, KOLORLINI, 2);
    listaPunktow = pierwszy;
    while(listaPunktow) {
        al_draw_filled_circle(listaPunktow->wspX, listaPunktow->wspY, ROZMIARPUNKTU, KOLORPUNKTU);
        al_draw_circle(listaPunktow->wspX, listaPunktow->wspY, ROZMIARPUNKTU, KOLOROBRAMOWANIAPUNKTU, 1);
        listaPunktow = listaPunktow->nastepny;
    }
    listaPunktow = pierwszy;
    while(listaPunktow) {
        al_draw_textf(circleFont, al_map_rgb(255, 255, 255), listaPunktow->wspX, listaPunktow->wspY-al_get_font_line_height(circleFont)/2, ALLEGRO_ALIGN_CENTER, "%d", number);
        number++;
        listaPunktow = listaPunktow->nastepny;
    }
}
double countPath(struct punkt *listaPunktow) {
    listaPunktow = listaPunktow->nastepny;
    if(!listaPunktow)
        //jezeli lista jest pusta
        return 0;
    double dlugoscTrasy = 0;
    struct punkt *miasto1 = NULL, *miasto2 = NULL, *pierwszeMiasto = listaPunktow;
    while(listaPunktow) {
        if(miasto1 == NULL) {
            miasto1 = listaPunktow;
            listaPunktow = listaPunktow->nastepny;
            continue;
        }
        if(miasto2 == NULL) {
            miasto2 = listaPunktow;
            listaPunktow = listaPunktow->nastepny;
        }
        dlugoscTrasy += calculateDistance(miasto1, miasto2);
        miasto1 = miasto2;
        miasto2 = NULL;
        if(!listaPunktow)
            break;
        listaPunktow = listaPunktow->nastepny;
    }
    miasto2 = pierwszeMiasto;
    dlugoscTrasy += calculateDistance(miasto1, miasto2);
    return dlugoscTrasy;
}
void printHowLong(double trasa, ALLEGRO_FONT *circleFont) {
    al_draw_filled_rectangle(0, WYSOKOSCOKNA-PASEKSTANU, SZEROKOSCOKNA, WYSOKOSCOKNA, CZARNY);
    if(trasa)
        al_draw_textf(circleFont, al_map_rgb(255, 255, 255), SZEROKOSCOKNA/2, WYSOKOSCOKNA-PASEKSTANU/2-al_get_font_line_height(circleFont)/2, ALLEGRO_ALIGN_CENTER, "Obliczona sciezka wynosi: %0.2lf", trasa);
}

