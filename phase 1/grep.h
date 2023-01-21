#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>

#include "find.h"

int getLine(char *address, int index)
{
    FILE *r = fopen(address, "r");
    int lines = 1;
    for (int i = 0; i < index; i++)
    {
        if (fgetc(r) == '\n')
            lines += 1;
    }
    fclose(r);
    return lines;
}

void printLine(char *address, int line)
{
    FILE *r = fopen(address, "r");
    fseek(r, 0, SEEK_END);
    long size = ftell(r);
    fseek(r, 0, SEEK_SET);
    long position = ftell(r);

    int seenLines = 1;

    while (seenLines < line && position < size)
    {
        if (fgetc(r) == '\n')
            seenLines++;
        position++;
    }

    char c = fgetc(r);
    while (c != '\n' && position < size)
    {
        printf("%c", c);
        c = fgetc(r);
        position++;
    }
    printf("\n");
    fclose(r);
    return;
}

void printAllLinesGrep(char *address, char *pattern)
{
    int line;
    int att[4] = {0, 0, 0, 1};
    struct linkedList *check = find(address, pattern, att);

    if (check->value == -10)
        return;

    if (check->value != -1)
    {
        line = getLine(address, check->value);
        printf("%s: ", address);
        printLine(address, line);
    }

    if (check->next == NULL)
        return;

    check = check->next;
    while (check->value != -10)
    {
        if (getLine(address, check->value) != line)
        {
            line = getLine(address, check->value);
            printf("%s: ", address);
            printLine(address, line);
        }

        if (check->next == NULL)
            break;
        check = check->next;
    }
    return;
}

int countAllLinesGrep(char *address, char *pattern)
{
    int line, count = 0;
    int att[4] = {0, 0, 0, 1};
    struct linkedList *check = find(address, pattern, att);

    if (check->value == -10)
        return count;

    if (check->value != -1)
    {
        count++;
        line = getLine(address, check->value);
    }

    if (check->next == NULL)
        return count;

    check = check->next;
    while (check->value != -10)
    {
        if (getLine(address, check->value) != line)
        {
            count++;
            line = getLine(address, check->value);
        }

        if (check->next == NULL)
            break;
        check = check->next;
    }
    return count;
}

int grep(char **addresses, int n, char *pattern, bool c, bool l)
{
    if (c && l)
        return 0;

    if (!c && !l)
    {
        for (int i = 0; i < n; i++)
            printAllLinesGrep(*(addresses + i), pattern);
    }

    if (c && !l)
    {
        int count = 0;
        for (int i = 0; i < n; i++)
            count += countAllLinesGrep(*(addresses + i), pattern);
        printf("%d\n", count);
    }

    if (!c && l)
    {
        for (int i = 0; i < n; i++)
        {
            if (countAllLinesGrep(*(addresses + i), pattern))
                printf("%s\n", *(addresses + i));
        }
    }

    return 1;
}

