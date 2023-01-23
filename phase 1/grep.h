#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>

//#include "find.h"

#define MAX_LINE_LENGTH 10000
#define MAX_ADDRESS_LENGTH 255

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

int countFileLines(char *address)
{
    FILE *read = fopen(address, "r");
    char c = fgetc(read);
    int count = 1;
    while (c != EOF)
    {
        if (c == '\n')
            count++;
        c = fgetc(read);
    }
    fclose(read);
    return count;
}

//void fprintLine(char *address, int line, char *outputName)
//{
//    FILE *r = fopen(address, "r");
//    char hiddenAddress[MAX_ADDRESS_LENGTH] = ".hidden/";
//    strcat(hiddenAddress, outputName);
//    FILE *write = fopen(hiddenAddress, "w");
//    fseek(r, 0, SEEK_END);
//    long size = ftell(r);
//    fseek(r, 0, SEEK_SET);
//    long position = ftell(r);
//
//    int seenLines = 1;
//
//    while (seenLines < line && position < size)
//    {
//        if (fgetc(r) == '\n')
//            seenLines++;
//        position++;
//    }
//
//    char c = fgetc(r);
//    while (c != '\n' && c != EOF)
//    {
//        fputc(c, write);
//        c = fgetc(r);
//        position++;
//    }
//    fclose(r);
//    fclose(write);
//    return;
//}

void textComparator(char *address1, char *address2)
{
    char line1[MAX_LINE_LENGTH];
    char line2[MAX_LINE_LENGTH];

    int countLines1 = countFileLines(address1);
    int countLines2 = countFileLines(address2);

    FILE *read1 = fopen(address1, "r");
    FILE *read2 = fopen(address2, "r");

    int i;

    for (i = 1; i <= countLines1 && i <= countLines2; i++)
    {
        fgets(line1, MAX_LINE_LENGTH, read1);
        fgets(line2, MAX_LINE_LENGTH, read2);
        if (line1[strlen(line1) - 1] == '\n')
            line1[strlen(line1) - 1] = '\0';
        if (line2[strlen(line2) - 1] == '\n')
            line2[strlen(line2) - 1] = '\0';

        if (strcmp(line1, line2))
        {
            printf("============ #%d ============\n%s\n%s\n", i, line1, line2);
        }
    }

    if (i <= countLines1)
    {
        printf(">>>>>>>>>>>> #%d - #%d >>>>>>>>>>>>\n", i, countLines1);
        while (i++ <= countLines1)
        {
            fgets(line1, MAX_LINE_LENGTH, read1);
            if (line1[strlen(line1) - 1] == '\n')
                line1[strlen(line1) - 1] = '\0';
            printf("%s\n", line1);
        }
    }

    if (i <= countLines2)
    {
        printf(">>>>>>>>>>>> #%d - #%d >>>>>>>>>>>>\n", i, countLines2);
        while (i++ <= countLines2)
        {
            fgets(line1, MAX_LINE_LENGTH, read2);
            if (line1[strlen(line1) - 1] == '\n')
                line1[strlen(line1) - 1] = '\0';
            printf("%s\n", line1);
        }
    }

    return;
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

