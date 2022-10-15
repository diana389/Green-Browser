#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {white, black, red, green, blue, yellow} color;

typedef struct
{
    char URL[201], titlu[201], *continut;
    int nr_accesari, checksum;
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

void check_style(char *cont, int poz_start_continut, site *v, int no_sites)
{
    char *style = strdup(cont);
    style[poz_start_continut] = '\0';
    //printf("\n%s\n%s", style, cont);

    char *end_color, background_color[100], color[100];
    char *p = strstr(style, "background-color:");
    if (p)
    {
        p = p + strlen("background-color:");
        end_color = strchr(p, ';');
        strncpy(background_color, p, end_color - p);
        background_color[end_color - p] = '\0';
        //printf("\n%s", background_color);
        v[no_sites - 1].bg_color = color_match(background_color);
        //printf("%d", v[no_sites-1].bg_color);
    }

    p = strstr(style, "color:");
    if (p && (p[-1] != '-'))
    {
        p = p + strlen("color:");
        end_color = strchr(p, ';');
        strncpy(color, p, end_color - p);
        color[end_color - p] = '\0';
        //printf("\n%s", color);
        v[no_sites - 1].text_color = color_match(color);
        //printf("%d", v[no_sites-1].text_color);
    }
    free(style);
}

void check_capacity(int no_sites, int *capacity, site *v)
{
    if (no_sites > *capacity)
    {
        *capacity = *capacity + 3;
        v = realloc(v, *capacity * sizeof(site));
    }
}

void complete_database(FILE *current_file, site *v, int no_sites)
{
    int lg;

    fscanf(current_file, "%s%d%d%d", v[no_sites - 1].URL, &lg, &v[no_sites - 1].nr_accesari, &v[no_sites - 1].checksum);
    //printf("\n%s %d %d %d", v[no_sites-1].URL, lg, v[no_sites-1].nr_accesari, v[no_sites-1].checksum);

    v[no_sites - 1].continut = malloc(lg * sizeof(char));
    char *cont = malloc(lg * sizeof(char));

    fgets(cont, lg, current_file);
    while (strstr(cont, "<title>") == NULL)
        fgets(cont, lg, current_file);
    strcpy(v[no_sites - 1].titlu, cont + 7);
    v[no_sites - 1].titlu[strlen(v[no_sites - 1].titlu) - 9] = '\0';
    //printf("\n%s", v[no_sites - 1].titlu);

    fgets(cont, lg, current_file);
    char *p = strchr(cont, '>');
    int poz_start_continut = p - cont + 1;

    check_style(cont, poz_start_continut, v, no_sites);

    strcpy(v[no_sites - 1].continut, cont + poz_start_continut);
    while (fgets(cont, lg, current_file))
        strcat(v[no_sites - 1].continut, cont);
        
    if (v[no_sites - 1].continut[strlen(v[no_sites - 1].continut) - 1] == '\n')
        v[no_sites - 1].continut[strlen(v[no_sites - 1].continut) - 1] = '\0';

    v[no_sites - 1].continut[strlen(v[no_sites - 1].continut) - 12] = '\0';
    //printf("\n%s", v[no_sites - 1].continut);

    free(cont);
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
        while (fscanf(master_file, "%s", file_name) != EOF)
        {
            //printf("\n%s", file_name);
            FILE *current_file = fopen(file_name, "r");
            if (current_file == NULL)
                exit(-1);

            if (current_file)
            {
                no_sites++;

                if (no_sites > capacity)
                {
                    capacity = capacity + 3;
                    v = realloc(v, capacity * sizeof(site));
                }
                //check_capacity(no_sites, &capacity, v);
                complete_database(current_file, v, no_sites);
            }
            fclose(current_file);
        }
    }

    for (i = 0; i < no_sites; i++)
    {
        printf("%s %d %s\n", v[i].URL, v[i].nr_accesari, v[i].titlu);
        free(v[i].continut);
    }

    free(v);
    fclose(master_file);
    return 0;
}
