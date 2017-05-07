#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define SZEROKOSCOKNA 640
#define WYSOKOSCOKNA 480

#define KOLORPUNKTU 37,134,108
#define ROZMIARPUNKTU 20

void narysujPunkt(int x, int y) {
    //rysuje punkt

    al_draw_filled_circle(x, y, ROZMIARPUNKTU, al_map_rgb(KOLORPUNKTU));
    al_draw_circle(x, y, ROZMIARPUNKTU, al_map_rgb(0, 0, 0), 2);
    al_flip_display();
}

int main(int argc, char **argv) {
    srand(time(NULL));

    if( al_init() && al_init_primitives_addon() ) {
        //sprawdzenie czy biblioteka allegro została prawidlowo zainicjowana

        ALLEGRO_DISPLAY *oknoKomiwojazera = al_create_display(SZEROKOSCOKNA, WYSOKOSCOKNA); //utworzenie okna
        if( oknoKomiwojazera ) {
            //sprawdzenie czy okno zostało prawidłowo utworzone

            al_set_window_title(oknoKomiwojazera, "Problem komiwojazera"); //zmiana tytułu okna

            al_clear_to_color(al_map_rgb(165, 201, 111));
            al_flip_display();
            al_rest(1.0); //wstrzymanie ekranu

            narysujPunkt(100, 150);
            al_flip_display();
            al_rest(1.0);

            al_destroy_display(oknoKomiwojazera);
        } else {
            //jeżeli nie mozna utworzyć okna

            printf("Nie udalo sie utworzyc okna");
            return -1;
        }
    } else {
        printf("Nie udalo sie zainicjowac biblioteki allegro");
        return -1;
    }
    return 0;
}
