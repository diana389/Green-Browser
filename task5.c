#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <unistd.h>

typedef enum {black, red, green, yellow, blue, white} color;

typedef struct
{
    char URL[201], titlu[201], *continut, *script;
    int nr_accesari, checksum, lg;
    color bg_color, text_color;
} site;

color color_match(char *color_to_match)
{
    if (strcmp(color_to_match, "white") == 0)
        return white;

    if (strcmp(color_to_match, "black") == 0)
        return black;

    if (strcmp(color_to_match, "red") == 0)
        return red;

    if (strcmp(color_to_match, "green") == 0)
        return green;

    if (strcmp(color_to_match, "blue") == 0)
        return blue;

    if (strcmp(color_to_match, "yellow") == 0)
        return yellow;

    return -1;
}

void check_style(char *cont, int poz_start_continut, site *current_site)
{
    char *style = strdup(cont);
    style[poz_start_continut] = '\0';

    current_site->bg_color = -1;
    current_site->text_color = -1;

    char *end_color, background_color[100], color[100];
    char *p = strstr(style, "background-color:");

    if (p)
    {
        p = p + strlen("background-color:");

        end_color = strchr(p, ';');
        strncpy(background_color, p, end_color - p);
        background_color[end_color - p] = '\0';

        current_site->bg_color = color_match(background_color);
    }

    p = strstr(style, "color:");

    if (p && (p[-1] != '-'))
    {
        p = p + strlen("color:");

        end_color = strchr(p, ';');
        strncpy(color, p, end_color - p);
        color[end_color - p] = '\0';

        current_site->text_color = color_match(color);
    }

    free(style);
}

void complete_database(FILE *current_file, site *current_site)
{
    fscanf(current_file, "%s%d%d%d", current_site->URL, &current_site->lg, &current_site->nr_accesari, &current_site->checksum);

    current_site->continut = malloc(current_site->lg * sizeof(char));
    current_site->script = malloc(current_site->lg * sizeof(char));
    strcpy(current_site->script, "");

    char *cont = malloc(current_site->lg * sizeof(char));

    fgets(cont, current_site->lg, current_file);

    while (strstr(cont, "<title>") == NULL)
    {
        fgets(cont, current_site->lg, current_file);
        strcat(current_site->script, cont);
    }

    strcpy(current_site->titlu, cont + 7);
    current_site->titlu[strlen(current_site->titlu) - 9] = '\0';

    fgets(cont, current_site->lg, current_file);
    strcat(current_site->script, cont);

    char *p = strchr(cont, '>');
    int poz_start_continut = p - cont + 1;

    check_style(cont, poz_start_continut, current_site);

    strcpy(current_site->continut, cont + poz_start_continut);

    while (fgets(cont, current_site->lg, current_file))
    {
        strcat(current_site->continut, cont);
        strcat(current_site->script, cont);
    }

    if (current_site->continut[strlen(current_site->continut) - 1] == '\n')
        current_site->continut[strlen(current_site->continut) - 1] = '\0';

    current_site->continut[strlen(current_site->continut) - 12] = '\0';
    current_site->script[strlen(current_site->script)] = '\0';

    free(cont);
}

int compFunc1(site site_1, site site_2)
{
    if (strcmp(site_1.continut, site_2.continut) > 0)
        return 1;

    if (strcmp(site_1.continut, site_2.continut) == 0)
        return 0;

    return -1;
}

int compFunc2(site site_1, site site_2)
{
    if (site_1.nr_accesari < site_2.nr_accesari)
        return 1;

    return -1;
}

void sort_database(site *v, int no_good_sites, int *vf2, int compFuncArgument(site, site))
{
    int i, ok = 1, aux;

    while (ok)
    {
        ok = 0;

        for (i = 0; i < no_good_sites - 1; i++)
        {
            if (compFuncArgument(v[vf2[i]], v[vf2[i + 1]]) == 1)
            {
                aux = vf2[i];
                vf2[i] = vf2[i + 1];
                vf2[i + 1] = aux;
                ok = 1;
            }
        }
    }

    ok = 1;

    while (ok)
    {
        ok = 0;

        for (i = 0; i < no_good_sites - 1; i++)
            if (compFuncArgument(v[i], v[i + 1]) == 0 && (v[i].nr_accesari < v[i + 1].nr_accesari))
            {
                aux = vf2[i];
                vf2[i] = vf2[i + 1];
                vf2[i + 1] = aux;
                ok = 1;
            }
    }
}

void print_site(site *v, int poz) // Subprogramul afiseaza site-ul conform scriptului.
{
    clear();

    int culbg = v[poz].bg_color, culfg = v[poz].text_color;

    if (culbg == 5)
        culbg = 7; // Codurile culorilor din ncurses corespund celor din enumerare, cu exceptia culorii alb.

    if (culfg == 5)
        culfg = 7;

    attron(A_BOLD);
    mvprintw(0, 0, v[poz].titlu); // Se afiseaza titlul in bold.
    attroff(A_BOLD);

    init_pair(1, culfg, culbg); // Se initializeaza perechea de culori.

    attron(COLOR_PAIR(1));
    mvprintw(2, 0, v[poz].continut); // Se afiseaza continutul.
    attroff(COLOR_PAIR(1));

    mvprintw(22, 34, "B - back");

    refresh();

    int input_letter;

    while (1)
    {
        input_letter = getch();

        if (input_letter == 'b') // Daca se apasa litera 'b' se revine in starea anterioara.
            break;
    }
}

void task2(site *v, int no_sites, char input1[100])
{
    clear();

    mvprintw(1, 1, input1);
    mvprintw(3, 23, "Choose from the following sites:");

    refresh();

    char input[100], *p, *pp; // task 2:
    int *vf, *vf2, no_good_sites = 0, i;

    strcpy(input, input1);

    vf = malloc(no_sites * sizeof(int));

    for (i = 0; i < no_sites; i++)
        vf[i] = 0;

    p = strtok(input, " ");

    while (p)
    {
        for (i = 0; i < no_sites; i++)
        {
            pp = strstr(v[i].continut, p);

            if (pp != NULL && isalpha(pp[-1]) == 0 && isalpha(pp[strlen(p)]) == 0)
                vf[i]++;
        }
        p = strtok(NULL, " ");
    }

    vf2 = malloc(no_good_sites * sizeof(int));

    for (i = 0; i < no_sites; i++)
    {
        if (vf[i] > 0)
        {
            vf2[no_good_sites] = i;
            no_good_sites++;
        }
    }

    sort_database(v, no_good_sites, vf2, compFunc1);

    int poz = 6;

    for (i = 0; i < no_good_sites; i++)
    {
        mvprintw(poz, 27, v[vf2[i]].URL); // Se afiseaza site-urile.
        poz = poz + 2;
    }

    mvprintw(22, 34, "B - back");
    move(6, 27);
    refresh();

    int input_letter, option = 0;

    while (1)
    {
        input_letter = getch();

        if (input_letter == 'b') // Daca se apasa litera 'b' se revine in starea anterioara.
            break;
        else
        {
            if (input_letter == KEY_DOWN) // Cursorul se muta pe optiunea urmatoare.
            {
                option++;

                if (option > no_good_sites - 1)
                    option = 0;

                move(6 + 2 * option, 27);
            }
            else
            {
                if (input_letter == KEY_UP) // Cursorul se muta pe optiunea anterioara.
                {
                    option--;
                    if (option < 0)
                        option = no_good_sites - 1;
                    move(6 + 2 * option, 27);
                }
                else
                {
                    if (input_letter == '\n')
                    {
                        print_site(v, vf2[option]); // Se acceseaza site-ul selectat.

                        clear();
                        mvprintw(1, 1, input1);
                        mvprintw(3, 23, "Choose from the following sites:");

                        int poz = 6;

                        for (i = 0; i < no_good_sites; i++)
                        {
                            mvprintw(poz, 27, v[vf2[i]].URL);
                            poz = poz + 2;
                        }

                        mvprintw(22, 34, "B - back");
                        move(6 + 2 * option, 27);

                        refresh();
                    }
                }
            }
        }
    }

    free(vf);
    free(vf2);
}

void task3(site *v, int no_sites, char input1[100])
{
    clear();
    refresh();

    mvprintw(1, 1, input1);
    mvprintw(3, 23, "Choose from the following sites:");

    char input[100], *p, *pp;
    int *vf, *vf2, no_good_sites = 0, i;

    strcpy(input, input1);

    vf = malloc(no_sites * sizeof(int));

    for (i = 0; i < no_sites; i++)
        vf[i] = 0;

    p = strtok(input, " ");

    while (p)
    {
        if (p[0] == '"')
        {
            char secv[101] = "";
            p = p + 1;

            strcat(secv, p);
            strcat(secv, " ");

            do
            {
                p = strtok(NULL, " ");
                strcat(secv, p);
                strcat(secv, " ");
            }while (p && (p[strlen(p) - 1] != '"'));

            secv[strlen(secv) - 2] = '\0';

            for (i = 0; i < no_sites; i++)
                if (strstr(v[i].continut, secv) != NULL)
                    vf[i]++;
        }
        else
        {
            if (p[0] == '-')
            {
                for (i = 0; i < no_sites; i++)
                    if (strstr(v[i].continut, p + 1) != NULL)
                        vf[i] = -100;
            }
            else
            {
                for (i = 0; i < no_sites; i++)
                {
                    pp = strstr(v[i].continut, p);

                    if (pp != NULL && isalpha(pp[-1]) == 0 && isalpha(pp[strlen(p)]) == 0)
                        vf[i]++;
                }
            }
        }

        p = strtok(NULL, " ");
    }

    vf2 = malloc(no_sites * sizeof(int));

    for (i = 0; i < no_sites; i++)
    {
        if (vf[i] > 0)
            vf2[no_good_sites++] = i;
    }

    sort_database(v, no_good_sites, vf2, compFunc2);

    int poz = 6;

    for (i = 0; i < no_good_sites; i++) // Se afiseaza site-urile.
    {
        mvprintw(poz, 27, v[vf2[i]].URL);
        poz = poz + 2;
    }

    mvprintw(22, 34, "B - back");

    move(6, 27);
    refresh();

    int input_letter, option = 0;

    while (1)
    {
        input_letter = getch();

        if (input_letter == 'b') // Daca se apasa litera 'b' se revine in starea anterioara.
            break;
        else
        {
            if (input_letter == KEY_DOWN) // Cursorul se muta pe optiunea urmatoare.
            {
                option++;

                if (option > no_good_sites - 1)
                    option = 0;

                move(6 + 2 * option, 27);
            }
            else
            {
                if (input_letter == KEY_UP) // Cursorul se muta pe optiunea anterioara.
                {
                    option--;

                    if (option < 0)
                        option = no_good_sites - 1;

                    move(6 + 2 * option, 27);
                }
                else
                {
                    if (input_letter == '\n')
                    {
                        print_site(v, vf2[option]); // Se acceseaza site-ul selectat.

                        clear();
                        mvprintw(1, 1, input1);
                        mvprintw(3, 23, "Choose from the following sites:");

                        int poz = 6;

                        for (i = 0; i < no_good_sites; i++)
                        {
                            mvprintw(poz, 27, v[vf2[i]].URL);
                            poz = poz + 2;
                        }

                        mvprintw(22, 34, "B - back");
                        move(6 + 2 * option, 27);

                        refresh();
                    }
                }
            }
        }
    }

    free(vf);
    free(vf2);
}

int main()
{
    int no_sites = 0, capacity = 3, i;
    site *v = malloc(capacity * sizeof(site));

    FILE *master_file = fopen("master.txt", "r");

    if (master_file == NULL)
        exit(-1);

    if (master_file)
    {
        char file_name[31];

        while (fscanf(master_file, "%s", file_name) != EOF) // Se citeste numele fisierelor pana la sfarsitul fisierului master.txt.
        {
            FILE *current_file = fopen(file_name, "r");

            if (current_file == NULL)
                exit(-1);

            if (current_file)
            {
                no_sites++; // Creste numarul site-urilor.

                if (no_sites > capacity)
                {
                    capacity = capacity + 3;
                    v = realloc(v, capacity * sizeof(site)); // Daca numarul site-urilor depaseste dimensiunea vectorului, se realoca.
                }

                complete_database(current_file, &v[no_sites - 1]);
            }
            fclose(current_file);
        }
    }

    fclose(master_file);

    initscr();
    noecho();
    keypad(stdscr, TRUE);

    start_color();
    use_default_colors();
    assume_default_colors(-1, -1);

    attron(A_BLINK | A_BOLD | A_UNDERLINE);
    mvprintw(10, 35, "PRESS C");
    attroff(A_BLINK | A_BOLD | A_UNDERLINE);
    move(0, 0);
    refresh();

    int input_letter, l = 0;
    char input[100] = "";

    while (1)
    {
        input_letter = getch();

        if (input_letter == 'c')
            break;
    }

    clear();
    refresh();

    mvprintw(10, 20, "+----------------------------------------+");
    mvprintw(12, 20, "+----------------------------------------+");
    mvprintw(11, 13, "search:|");
    mvprintw(11, 61, "|");

    move(11, 21);
    refresh();

    while (1)
    {
        input_letter = getch();

        if (input_letter == '\n') // Se blocheaza textul.
        {
            attron(A_BLINK);
            mvprintw(11, 21, input);
            attroff(A_BLINK);
            mvprintw(22, 10, "Q - quit;     S - cautare simpla;     A - cautare avansata");
            refresh();

            input_letter = getch();

            if (input_letter == 'q')
            {
                mvprintw(15, 20, "                     ");
                mvprintw(15, 25, "Are you sure you want to exit?");
                mvprintw(17, 25, "      Yes(Y)     No(N)");
                refresh();

                while (1)
                {
                    input_letter = getch();

                    if (input_letter == 'y')
                    {
                        endwin();

                        for (i = 0; i < no_sites; i++)
                        {
                            free(v[i].continut);
                            free(v[i].script);
                        }

                        free(v);
                        return 0;
                    }
                    if (input_letter == 'n')
                    {
                        mvprintw(15, 20, "                                     ");
                        mvprintw(17, 20, "                                     ");
                        refresh();

                        break;
                    }
                }
            }

            if (input_letter == 's') // cautare simpla
            {
                task2(v, no_sites, input);

                clear();
                mvprintw(10, 20, "+----------------------------------------+");
                mvprintw(12, 20, "+----------------------------------------+");
                mvprintw(11, 14, "input:|");
                mvprintw(11, 61, "|");
                mvprintw(11, 21, input);
                refresh();
            }

            if (input_letter == 'a') // cautare avansata
            {
                task3(v, no_sites, input);

                clear();
                mvprintw(10, 20, "+----------------------------------------+");
                mvprintw(12, 20, "+----------------------------------------+");
                mvprintw(11, 14, "input:|");
                mvprintw(11, 61, "|");
                mvprintw(11, 21, input);
                refresh();
            }

            mvprintw(22, 10, "                                                            ");
        }
        else
        {
            if (input_letter == KEY_BACKSPACE)
            {
                mvprintw(15, 20, "You pressed backspace");

                if (l > 0)
                {
                    l--;
                    input[l] = '\0';
                }

                mvprintw(11, 21, "                                        ");
                mvprintw(11, 21, input);

                refresh();
            }
            else
            {
                if (l < 40)
                {
                    mvprintw(15, 20, "                     ");
                    mvprintw(11, 21, "                                       ");

                    input[l++] = input_letter;
                    input[l] = '\0';
                    mvprintw(11, 21, input);

                    refresh();
                }
            }
        }
    }
}