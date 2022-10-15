#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef enum {white, black, red, green, blue, yellow} color;

typedef struct
{
    char URL[201], titlu[201], *continut, *script;
    int nr_accesari, checksum, lg;
    color bg_color, text_color;
} site;

color color_match (char *color_to_match)
{
    if(strcmp(color_to_match, "white")==0)
        return white;
    if(strcmp(color_to_match, "black")==0)
        return black;
    if(strcmp(color_to_match, "red")==0)
        return red;
    if(strcmp(color_to_match, "green")==0)
        return green;
    if(strcmp(color_to_match, "blue")==0)
        return blue;
    if(strcmp(color_to_match, "yellow")==0)
        return yellow;
    return -1;
}

void check_style (char *cont, int poz_start_continut, site *current_site)
{
    char *style = strdup(cont);
    style[poz_start_continut] = '\0';

    current_site->bg_color = -1;
    current_site->text_color = -1;

    char *end_color, background_color[100], color[100];
    char *p = strstr(style, "background-color:");
    if(p)
    {
        p = p + strlen("background-color:");
        end_color = strchr(p, ';');
        strncpy(background_color, p, end_color-p);
        background_color[end_color-p] = '\0';
        current_site->bg_color = color_match(background_color);
    }

    p = strstr(style, "color:");
    if(p && (p[-1] != '-'))
    {
        p = p + strlen("color:");
        end_color = strchr(p, ';');
        strncpy(color, p, end_color-p);
        color[end_color-p] = '\0';
        current_site->text_color = color_match(color);
    }
    free(style);
    //printf("culori: %d %d \n\n", current_site->bg_color, current_site->text_color);
}

void check_capacity (int no_sites, int *capacity, site *v)
{
    if(no_sites > *capacity)
    {
        *capacity = *capacity + 3;
        v = realloc(v, *capacity * sizeof(site));
    }
}

void complete_database (FILE *current_file, site *current_site)
{
    fscanf(current_file, "%s%d%d%d", current_site->URL, &current_site->lg, &current_site->nr_accesari, &current_site->checksum);

    current_site->continut = calloc((current_site->lg + 1), sizeof(char));
    current_site->script = calloc((current_site->lg + 1), sizeof(char));
    //strcpy(current_site->script, "");

    char *cont = calloc((current_site->lg + 1), sizeof(char));

    fgets(cont, current_site->lg, current_file);
    while(strstr(cont, "<title>") == NULL)
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

    strcpy(current_site->continut, cont+poz_start_continut);
    while(fgets(cont, current_site->lg, current_file))
    {
        strcat(current_site->continut, cont);
        strcat(current_site->script, cont);
    }

    if(current_site->continut[strlen(current_site->continut) - 1] == '\n')
    current_site->continut[strlen(current_site->continut) - 1] = '\0';
     
    current_site->continut[strlen(current_site->continut) - 12] = '\0';
    current_site->script[current_site->lg] = '\0';

    //printf("\n%s %s %d %s %d %d %s %d %d", current_site->URL, current_site->titlu, current_site->text_color, current_site->script, current_site->nr_accesari, current_site->lg, current_site->continut, current_site->checksum, current_site->bg_color);

    free(cont);
}

int power (int x)
{
    int i, n=1;
    for(i=0;i<x;i++)
        n=n*2;
    return n;
}

int rotr(char x, int k)
{
    while(k >= 8)
        k = k - 8;
    int i, masca = 0;
    for(i=0; i<k; i++)
        masca = masca + power(i);
    masca = masca & x;
    masca = masca << (8 - k);
    x = x >> k;
    return x ^ masca;
}

int rotl(char x, int k)
{
    while(k >= 8)
        k = k - 8;
    int i, masca = 0;
    for(i=0; i<8-k; i++)
        masca = masca + power(i);
    masca = masca & x;
    masca = masca << k;
    x = x >> (8 - k);
    return x ^ masca;
}

int main()
{
    int no_sites = 0, capacity = 3, i, j;
    site *v = malloc(capacity * sizeof(site));

    FILE *master_file = fopen("master.txt", "r");
    if(master_file == NULL)
        exit(-1);

    if(master_file)
    {
        char file_name[31];
        while(fscanf(master_file, "%s", file_name) != EOF)
        {
            FILE *current_file = fopen(file_name, "r");
            if(current_file == NULL)
                exit(-1);

            if(current_file)
            {
                no_sites++;
                if(no_sites > capacity)
                {
                    capacity = capacity + 3;
                    v = realloc(v, capacity * sizeof(site));
                }
                //check_capacity(no_sites, &capacity, v);
                complete_database(current_file, &v[no_sites-1]);
            }
            fclose(current_file);
        }
    }

    //for(i=0;i<no_sites;i++)
      //  printf("\ncontinut: %s\nscript: %s", v[i].continut, v[i].script);
    char file_name[101];

    while(fgets(file_name, 101, stdin))
    {
        int found=0;
        file_name[strlen(file_name)-1]='\0';

        for(i=0;i<no_sites;i++)
            if(strcmp(v[i].URL, file_name)==0)
            {
                found=1;
                //printf("\nscript:%s\n", v[i].script);
                int checksum_found = 0;
                for(j=0; j<strlen(v[i].script); j++)
                    if(j%2)
                        {
                            checksum_found = checksum_found ^ rotr(v[i].script[j], j);
                            //printf("%c=%d\n", current_site.script[i], rotr(current_site.script[i], i));
                        }
                    else
                        {
                            checksum_found = checksum_found ^ rotl(v[i].script[j], j);
                            //printf("%c=%d\n", v[no_sites-1].script[i], rotr(v[no_sites-1].script[i], i));
                        }
                if(checksum_found == v[i].checksum)
                    printf("Website safe!\n");
                else printf("Malicious website! Official key: %d. Found key: %d\n", v[i].checksum, checksum_found);
                break;
            }

        if(found==0)
            printf("Website not found!\n");

        //printf("%d %d %d", v[0].lg, strlen(v[0].continut), strlen(v[0].script));
        //printf("\n%d\n", rotr('h', 1));
    }
    for(i = 0; i < no_sites; i++)
        {
            free(v[i].continut);
            free(v[i].script);
        }
    free(v);
    fclose(master_file);
    return 0;
}