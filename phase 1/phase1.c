#include<stdio.h>

void createNewFile(char *address)
{
    FILE *f = fopen(address, "w");
    fclose(f);
    return;
}

void copyFile(char *fromAdress, char *toAdress)
{
    FILE *from = fopen(fromAdress, "r");
    FILE *to = fopen(toAdress, "w");

    while (1)
    {
        char c = fgetc(from);
        if (c == EOF)
            break;
        fputc(c, to);
    }

    fclose(from);
    fclose(to);
    return;
}

void insert(char *address, int line, int pos, char *text)
{
    FILE *r = fopen(address, "r");
    FILE *w = fopen("tmp.txt", "w");
    int i = 1, j = 0;
    while (1)
    {
        if (i == line)
        {
            while (1)
            {
                if (j == pos)
                    fputs(text, w);
                char c = fgetc(r);
                if (c == EOF)
                    break;
                fputc(c, w);
                if (c == '\n')
                    break;
                j++;
            }

        }
        char c = fgetc(r);
        if (c == EOF)
            break;
        fputc(c, w);
        if (c == '\n')
            i++;
    }
    fclose(r);
    fclose(w);
    copyFile("tmp.txt", address);

    return;
}

void cat(char *address)
{
    FILE *f = fopen(address, "r");
    while(1)
    {
        char c = fgetc(f);
        if (c == EOF)
            break;
        printf("%c", c);
    }
    printf("\n");
    fclose(f);
    return;
}


int main()
{
    createNewFile("test.txt");
    insert("test.txt", 1, 0, "Hello\nHow are you?");
    cat("test.txt");
    cat("test.txt");
    return 17;
}
