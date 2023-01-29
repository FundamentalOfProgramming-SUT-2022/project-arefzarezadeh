#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include<sys/stat.h>

#define CAPACITY 255
#define MAX_STRING_SIZE 10000

struct pos
{
    int line;
    int position;
};

struct pos getPosFromIndex(char *address, int index)
{
    FILE *r = fopen(address, "r");
    struct pos p = {-1, 0};
    if (index >= 0)
        p.line = 1;
    for (int i = 0; i < index; i++)
    {
        char c = fgetc(r);
        if (c == EOF)
        {
            p.line = -1;
            return p;
        }
        else if (c == '\n')
        {
            p.line += 1;
            p.position = 0;
        }
        else
            p.position += 1;
    }
    fclose(r);
    return p;
}

struct pos getWordPosFromIndex(char *address, int index)
{
    FILE *r = fopen(address, "r");
    bool space = true;
    struct pos p = {1, 0};;
    for (int i = 0; i < index; i++)
    {
        char c = fgetc(r);
        if (c == EOF)
        {
            p.line = -1;
            return p;
        }
        else if (c == '\n')
        {
            space = true;
            p.line += 1;
            p.position = 0;
        }
        else if (c == ' ' || c == '\t')
            space = true;
        else if (space)
        {
            space = false;
            p.position += 1;
        }
        else
            space = false;

    }
    fclose(r);
    return p;
}

void printfFormattedLinkedList(char *address, struct linkedList *list, struct pos (*function) (char *, int), char *output)
{
    char tmp[MAX_STRING_SIZE] = {0};

    struct linkedList *check = list;
    if (check->value == -10)
        return;
    if (check->value == -1)
        sprintf(output, "-1\n");
    else
    {
        sprintf(tmp, "line:%d, position:%d\n", function(address, check->value).line, function(address, check->value).position);
        strcat(output, tmp);
    }
    if (check->next == NULL)
        return;
    check = check->next;
    while (check->value != -10)
    {
        sprintf(tmp, "line:%d, position:%d\n", function(address, check->value).line, function(address, check->value).position);
        strcat(output, tmp);
        if (check->next == NULL)
            break;
        check = check->next;
    }
    return;
}

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

char *readFile(char *address)
{
    FILE *read = fopen(address, "r");
    fseek(read, 0, SEEK_END);
    long size = ftell(read);
    fseek(read, 0, SEEK_SET);

    char *output = (char *) calloc(size + 2, sizeof(char));
    int i = 0;

    while (1)
    {
        char c = fgetc(read);
        if (c == EOF)
            break;
        output[i++] = c;
    }
    fclose(read);

    return output;
}

char firstNonSpaceChar(FILE *r, long position)
{
    //printf("%d\n", position);
    char c = fgetc(r);
    while (c == ' ')
        c = fgetc(r);
        //printf("this char is |%c| at |%ld|\n", c, position);
    fseek(r, position, SEEK_SET);
    return c;
}

bool fileExists(char *address)
{
    struct stat file;
    return (stat(address, &file) == 0);
}

bool createNewFile(char *address)
{
    if (fileExists(address))
        return false;
    char addressSoFar[CAPACITY] = {0};
    int i = 0;
    while (address[i] != '\0')
    {
        if (address[i] == '/')
        {
            if (!fileExists(addressSoFar))
                mkdir(addressSoFar);
        }
        addressSoFar[i] = address[i];
        i++;
    }
    FILE *f = fopen(address, "w");
    fclose(f);
    return true;
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

void undoHandler(char *address)
{
    char hidden[CAPACITY] = ".hidden/";
    strcat(hidden, address);
    createNewFile(hidden);
    copyFile(address, hidden);
//    FILE *f = fopen(".hidden/undo_file_path.txt", "w");
//    fputs(address, f);
//    fclose(f);
    return;
}

bool undo(char *address)
{
//    char address[CAPACITY];
//    FILE *r = fopen(".hidden/undo_file_path.txt", "r");
//
//    if (r == NULL)
//        return false;
//
//    fgets(address, CAPACITY, r);
//    fclose(r);
//
//    if (!strcmp(address, ""))
//        return false;
    char hidden[CAPACITY] = ".hidden/";
    strcat(hidden, address);
    if (!fileExists(hidden))
        return false;

    copyFile(address, ".hidden/tmp.txt");
    copyFile(hidden, address);
    copyFile(".hidden/tmp.txt", hidden);

    return true;
}

void insert(char *address, struct pos p, char *text)
{
    undoHandler(address);
    FILE *r = fopen(address, "r");
    FILE *w = fopen(".hidden/tmp.txt", "w");
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
    copyFile(".hidden/tmp.txt", address);

    return;
}

void cat(char *address, char *output)
{
    FILE *f = fopen(address, "r");
    char tmp[5] = {0};
    while(1)
    {
        char c = fgetc(f);
        if (c == EOF)
            break;
        sprintf(tmp, "%c", c);
        strcat(output, tmp);
    }
    strcat(output, "\n");
    fclose(f);
    return;
}

void removestr(char *address, struct pos p, int size, bool forward)
{
    undoHandler(address);
    int position = getPos(address, p), k = 0;
    FILE *read = fopen(address, "r");
    FILE *tmp = fopen(".hidden/tmp.txt", "w");

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
    copyFile(".hidden/tmp.txt", address);
    return;
}

void copystr(char *address, struct pos p, int size, bool forward)
{
    int position = getPos(address, p);
    FILE *read = fopen(address, "r");
    FILE *clipboard = fopen(".hidden/clipboard.txt", "w");
    int startingPos, endingPos;

    if (!forward)
    {
        startingPos = position - size;
        endingPos = position - 1;
    }
    else
    {
        startingPos = position;
        endingPos = position + size - 1;
    }

    for (int i = 0; i < startingPos; i++)
        fgetc(read);
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
    undoHandler(address);
    copystr(address, p, size, forward);
    removestr(address, p, size, forward);
    return;
}

void pastestr(char *address, struct pos p)
{
    undoHandler(address);
    FILE *r = fopen(".hidden/clipboard.txt", "r");
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

void insertTab(char *address, int position, int tab_count)
{
    char spaces[4 * tab_count + 1];
    for (int i = 0; i < 4 * tab_count; i++)
        spaces[i] = ' ';
    spaces[tab_count * 4] = '\0';
    insert(address, getPosFromIndex(address, position), spaces);
    return;
}

void auto_indent(char *address)
{
    undoHandler(address);

    FILE *write = fopen(".hidden/tmp_indent.txt", "w");
    FILE *read = fopen(address, "r");

    char previous2 = '\0';
    char previous = '\0';
    char current = '\0';

    int count_curly_brackets = 0;
    int count_white_space = 0;
    bool new_line = true;

    while (1)
    {
        if (new_line)
        {
            current = fgetc(read);
            while (current == ' ' || current == '\t')
                current = fgetc(read);

            if (current == EOF)
                break;

            if (current == '\n')
            {
                if (previous != '{' && previous2 != '}')
                    fputc('\n', write);
                previous2 = previous;
                previous = current;
                continue;
            }
            else
                new_line = false;

            if (current == '}')
                count_curly_brackets--;

            for (int i = 0; i < 4 * count_curly_brackets; i++)
                fputc(' ', write);

            if (current == '{')
            {
                count_curly_brackets++;
                fputc('{', write);
                fputc('\n', write);
                previous2 = '{';
                previous = '\n';
                new_line = true;
                continue;
            }

            if (current == '}')
            {
                if (previous != '\n')
                {
                    fputc('\n', write);
                    previous2 = previous = '\0';
                    new_line = true;
                    for (int i = 0; i < 4 * count_curly_brackets; i++)
                        fputc(' ', write);
                }

                fputc('}', write);
                fputc('\n', write);
                previous2 = '}';
                previous = '\n';
                new_line = true;
                continue;
            }

            fputc(current, write);
            new_line = false;
            previous2 = previous;
            previous = current;
            continue;
        }

        current = fgetc(read);

        if (current == EOF)
            break;

        if (current == ' ')
        {
            count_white_space++;
            new_line = false;
            fputc(current, write);
            previous2 = previous;
            previous = current;
            continue;
        }
        else if (current != '{')
            count_white_space = 0;

        if (current == '\n')
        {
            new_line = true;
            fputc(current, write);
            previous2 = previous;
            previous = current;
            continue;
        }

        if (current != '{' && current != '}')
        {
            new_line = false;
            fputc(current, write);
            previous2 = previous;
            previous = current;
            continue;
        }

        if (current == '{')
        {
            count_curly_brackets++;
            new_line = true;
            if (count_white_space == 0)
                fputc(' ', write);
            else if (count_white_space > 1)
                fseek(write, 1 - count_white_space, SEEK_CUR);
            fputc(current, write);
            fputc('\n', write);
            previous2 = previous;
            previous = current;
            continue;
        }

        if (current == '}')
        {
            count_curly_brackets--;
            new_line = true;
            fputc('\n', write);
            for (int i = 0; i < 4 * count_curly_brackets; i++)
                fputc(' ', write);
            fputc(current, write);
            fputc('\n', write);
            previous2 = current;
            previous = '\n';
            continue;
        }
    }

    fclose(read);
    fclose(write);
    copyFile(".hidden/tmp_indent.txt", address);
    return;
}

bool replace(char *address, char *textToBeFound, char *replacementText, int attributes[2])
{
    undoHandler(address);

    int att[4] = {0};
    FILE *read = fopen(address, "r");

    if (attributes[0])
    {
        int count = 1;
        att[1] = attributes[0];
        int ans = 0;
        while (count++ < attributes[0])
        {
            findNormal(read, textToBeFound) + 1;
        }

        ans = getRealPos(read);
        int x = findNormal(read, textToBeFound);
        //printf("%d\n", x);
        if (x == -1)
            return false;
        ans += x;

        int length = getRealPos(read) - ans;
        fclose(read);

        insert(address, getPosFromIndex(address, ans), replacementText);
        removestr(address, getPosFromIndex(address, ans + strlen(replacementText)), length, true);
        return true;
    }

    if (attributes[1])
    {
        int x = findNormal(read, textToBeFound);
        //printf("%d\n", x);
        if (x == -1)
            return true;

        int length = getRealPos(read) - x;


        //printf("x:%d\ngrp:%d\nlen:%d\n", x, getRealPos(read), length);

        fclose(read);

        insert(address, getPosFromIndex(address, x), replacementText);
        removestr(address, getPosFromIndex(address, x + strlen(replacementText)), length, true);
        return replace(address, textToBeFound, replacementText, attributes);
    }

    int x = findNormal(read, textToBeFound);
    if (x == -1)
        return true;

    int length = getRealPos(read) - x;
    fclose(read);

    insert(address, getPosFromIndex(address, x), replacementText);
    removestr(address, getPosFromIndex(address, x + strlen(replacementText)), length, true);
}
