//#include <C:\\Users\\Aaaa\\Desktop\\C\\poli\\tema 2\\task1\\main.c>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

    char *end_color, background_color[100], color[100];
    char *p = strstr(style, "background-color:");

    if (p)
    {
        p = p + strlen("background-color:");
        end_color = strchr(p, ';');

        strncpy(background_color, p, end_color - p);
        background_color[end_color - p] = '\0';
        v[no_sites - 1].bg_color = color_match(background_color);
    }

    p = strstr(style, "color:");

    if (p && (p[-1] != '-'))
    {
        p = p + strlen("color:");
        end_color = strchr(p, ';');

        strncpy(color, p, end_color - p);
        color[end_color - p] = '\0';
        v[no_sites - 1].text_color = color_match(color);
    }
    free(style);
}

void check_capacity(int no_sites, int *capacity, site *v)
{

    if (no_sites > *capacity)
    {

        *capacity = *capacity + 3;
        v = realloc(v, *capacity * sizeof(site));
        printf("\n\n%d %d", no_sites, *capacity);
    }
}

void complete_database(FILE *current_file, site *v, int no_sites)
{
    int lg;

    fscanf(current_file, "%s%d%d%d", v[no_sites - 1].URL, &lg, &v[no_sites - 1].nr_accesari, &v[no_sites - 1].checksum);

    v[no_sites - 1].continut = malloc(lg * sizeof(char));
    char *cont = malloc(lg * sizeof(char));

    fgets(cont, lg, current_file);
    while (strstr(cont, "<title>") == NULL)
        fgets(cont, lg, current_file);
    strcpy(v[no_sites - 1].titlu, cont + 7);
    v[no_sites - 1].titlu[strlen(v[no_sites - 1].titlu) - 9] = '\0';

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

void sort_database(site *v, int no_good_sites, int *vf2, int compFuncArgument(site, site))
{
    int i, ok = 1, aux;
    while (ok)
    {
        ok = 0;
        for (i = 0; i < no_good_sites - 1; i++)
        {
            //printf("\n%s %s", v[vf2[i]].continut, v[vf2[i+1]].continut);
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

    //for(i = 0; i < no_sites; i++)
    //    printf("%s %d %s\n", v[i].URL, v[i].nr_accesari, v[i].continut);

    char input[101], *p, *pp;
    int *vf, *vf2, no_good_sites = 0;

    vf = malloc(no_sites * sizeof(int));
    for (i = 0; i < no_sites; i++)
        vf[i] = 0;

    fgets(input, 101, stdin);
    input[strlen(input) - 1] = '\0';

    p = strtok(input, " ");

    while (p)
    {
        for (i = 0; i < no_sites; i++)
        {
            pp = strstr(v[i].continut, p);
            if (pp != NULL && isalpha(pp[-1]) == 0 && isalpha(pp[strlen(p)]) == 0)
                vf[i]++;
        }
        //if(strstr(v[i].continut, p) != NULL)
        //  vf[i]++;

        p = strtok(NULL, " ");
    }

    vf2 = malloc(no_good_sites * sizeof(int));

    for (i = 0; i < no_sites; i++)
    {
        //printf("%d ", vf[i]);
        if (vf[i] > 0)
        {
            vf2[no_good_sites] = i;
            no_good_sites++;
        }
    }

    sort_database(v, no_good_sites, vf2, compFunc1);

    /*printf("\n");
    for(i = 0; i < no_good_sites; i++)
        printf("\n%d ", vf2[i]);*/

    for (i = 0; i < no_good_sites; i++)
        printf("%s\n", v[vf2[i]].URL);

    for (i = 0; i < no_sites; i++)
        free(v[i].continut);

    free(v);
    free(vf);
    free(vf2);
    fclose(master_file);
    return 0;
}
