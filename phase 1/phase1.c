#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
//#include<windows.h>

#include "find.h"
#include "grep.h"

#define CAPACITY 255

struct pos
{
    int line;
    int position;
};

struct pos getPosFromIndex(char *address, int index)
{
    FILE *r = fopen(address, "r");
    struct pos p = {1, 0};;
    for (int i = 0; i < index; i++)
    {
        if (fgetc(r) == '\n')
        {
            p.line += 1;
            p.position = 0;
        }
        else
            p.position += 1;
    }
    fclose(r);
    return p;
};

int getPos(char *address, struct pos p)
{
    int position = 0, i = 1, j = 0;
    FILE *read = fopen(address, "r");
    while (1)
    {
        if (i == p.line)
        {
            while (1)
            {
                if (j == p.position)
                    break;
                char c = fgetc(read);
                if (c == EOF)
                    break;
                if (c == '\n')
                    break;
                j++;
                position++;
            }
            break;
        }
        char c = fgetc(read);
        if (c == EOF)
            break;
        if (c == '\n')
            i++;
        position++;
    }
    return position;
}

//void startApp()
//{
//    mkdir(".hidden files");
//    int att = GetFileAttributes(".hidden files");
//    SetFileAttributes(".hidden files", att + FILE_ATTRIBUTE_HIDDEN);
//}

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

void insert(char *address, struct pos p, char *text)
{
    FILE *r = fopen(address, "r");
    FILE *w = fopen("tmp.txt", "w");
    int i = 1, j = 0;
    while (1)
    {
        if (i == p.line)
        {
            while (1)
            {
                if (j == p.position)
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

void removestr(char *address, struct pos p, int size, bool forward)
{
    int position = getPos(address, p), k = 0;
    FILE *read = fopen(address, "r");
    FILE *tmp = fopen("tmp.txt", "w");

    while (1)
    {
        char c = fgetc(read);
        if (c == EOF)
            break;
        if (forward && !(k >= position && k < position + size))
            fputc(c, tmp);
        else if ((!forward) && !(k < position && k >= position - size))
            fputc(c, tmp);
        k++;
    }

    fclose(read);
    fclose(tmp);
    copyFile("tmp.txt", address);
    return;
}

void copystr(char *address, struct pos p, int size, bool forward)
{
    int position = getPos(address, p);
    FILE *read = fopen(address, "r");
    FILE *clipboard = fopen("clipboard.txt", "w");
    int startingPos, endingPos;

    if (!forward)
    {
        startingPos = position - size;
        endingPos = position - 1;
    }
    else
    {
        startingPos = position + 1;
        endingPos = position + size;
    }

    fseek(read, startingPos, SEEK_SET);
    int k = startingPos;
    while (1)
    {
        char c = fgetc(read);
        if (c == EOF)
            break;
        if (k >= startingPos && k <= endingPos)
            fputc(c, clipboard);
        k++;
    }
    fclose(read);
    fclose(clipboard);
    return;
}

void cutstr(char *address, struct pos p, int size, bool forward)
{
    copystr(address, p, size, forward);
    removestr(address, p, size, forward);
}

void pastestr(char *address, struct pos p)
{
    FILE *r = fopen("clipboard.txt", "r");
    fseek(r, 0, SEEK_END);
    int size = ftell(r);
    fseek(r, 0, SEEK_SET);

    char *text = calloc((size - 1) , sizeof(char));

    for (int i = 0; i < size - 1; i++)
        text[i] = fgetc(r);

    fclose(r);
    insert(address, p, text);

    return;
}

int main()
{
    createNewFile("test.txt");
    struct pos start = {1, 0};

    //FILE *x = fopen("test.txt", "r");

    int att[4] = {0, 0, 0, 1};

    insert("test.txt", start, "b b b b Salam\nKhobi?\nchert Khobi? chert a a b chert2 a chert chert3");
    printLinkedList(find("test.txt", "b", att));

    //printLine("test.txt", 3);
    struct pos x = getPosFromIndex("test.txt", 21);
    printf("\nline:%d\nposition:%d\n", x.line, x.position);
    return 17;
}
