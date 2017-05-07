#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int main(int argc, char **argv) {
    srand(time(NULL));

    if( al_init() && al_init_primitives_addon() ) {
        //sprawdzenie czy biblioteka allegro została prawidlowo zainicjowana

        ALLEGRO_DISPLAY *oknoKomiwojazera = al_create_display(640, 480); //utworzenie okna
        if( oknoKomiwojazera ) {
            //sprawdzenie czy okno zostało prawidłowo utworzone

            al_set_window_title(oknoKomiwojazera, "Problem komiwojazera"); //zmiana tytułu okna

            al_clear_to_color(al_map_rgb(165, 201, 111));
            al_flip_display();
            al_rest(1.0); //wstrzymanie ekranu

            int x, y;
            for(int i = 0; i < 100; i++) {
                x = rand() % 640;
                y = rand() % 480;

                al_draw_filled_circle(x, y, 20, al_map_rgb(37, 134, 108));
                al_draw_circle(x, y, 20, al_map_rgb(0, 0, 0), 1);
                al_flip_display();
                al_rest(0.1);
            }

            al_draw_filled_circle(50.0, 50.0, 20.0, al_map_rgb(37, 134, 108));
            al_flip_display();
            al_rest(2.0);

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
