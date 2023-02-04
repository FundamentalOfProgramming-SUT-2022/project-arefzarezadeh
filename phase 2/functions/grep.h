#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>

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

void textComparator(char *address1, char *address2, char *output)
{
    char line1[MAX_LINE_LENGTH];
    char line2[MAX_LINE_LENGTH];

    char tmp[MAX_LINE_LENGTH] = {0};

    int countLines1 = countFileLines(address1);
    int countLines2 = countFileLines(address2);

    FILE *read1 = fopen(address1, "r");
    FILE *read2 = fopen(address2, "r");

    int i;

    for (i = 1; i <= countLines1 && i <= countLines2; i++)
    {
        line1[0] = '\0';
        line2[0] = '\0';
        fgets(line1, MAX_LINE_LENGTH, read1);
        fgets(line2, MAX_LINE_LENGTH, read2);
        if (line1[strlen(line1) - 1] == '\n')
            line1[strlen(line1) - 1] = '\0';
        if (line2[strlen(line2) - 1] == '\n')
            line2[strlen(line2) - 1] = '\0';

        if (strcmp(line1, line2))
        {
            sprintf(tmp, "============ #%d ============\n%s\n%s\n", i, line1, line2);
            strcat(output, tmp);
        }
    }

    if (i <= countLines1)
    {
        sprintf(tmp, "<<<<<<<<<<<< #%d - #%d <<<<<<<<<<<<\n", i, countLines1);
        strcat(output, tmp);
        while (i++ <= countLines1)
        {
            line1[0] = '\0';
            fgets(line1, MAX_LINE_LENGTH, read1);
            if (line1[strlen(line1) - 1] == '\n')
                line1[strlen(line1) - 1] = '\0';
            sprintf(tmp, "%s\n", line1);
            strcat(output, tmp);
        }
    }

    if (i <= countLines2)
    {
        sprintf(tmp, ">>>>>>>>>>>> #%d - #%d >>>>>>>>>>>>\n", i, countLines2);
        strcat(output, tmp);
        while (i++ <= countLines2)
        {
            line1[0] = '\0';
            fgets(line1, MAX_LINE_LENGTH, read2);
            if (line1[strlen(line1) - 1] == '\n')
                line1[strlen(line1) - 1] = '\0';
            sprintf(tmp, "%s\n", line1);
            strcat(output, tmp);
        }
    }

    return;
}

void printLine(char *address, int line, char *output)
{
    FILE *r = fopen(address, "r");
    fseek(r, 0, SEEK_END);
    long size = ftell(r);
    fseek(r, 0, SEEK_SET);
    long position = ftell(r);
    char tmp[MAX_LINE_LENGTH] = {0};

    int seenLines = 1;

    while (seenLines < line && position < size - seenLines + 1)
    {
        if (fgetc(r) == '\n')
            seenLines++;
        position++;
    }

    char c = fgetc(r);
    while (c != '\n' && position < size - seenLines + 1)
    {
        sprintf(tmp, "%c", c);
        strcat(output, tmp);
        c = fgetc(r);
        position++;
    }
    strcat(output, "\n");
    fclose(r);
    return;
}

void printAllLinesGrep(char *address, char *pattern, char *output)
{
    int line;
    int att[4] = {0, 0, 0, 1};
    struct linkedList *check = find(address, pattern, att);
    char tmp[MAX_LINE_LENGTH] = {0};

    if (check->value == -10)
        return;

    if (check->value != -1)
    {
        line = getLine(address, check->value);
        sprintf(tmp, "%s: ", address);
        strcat(output, tmp);
        printLine(address, line, output);
    }

    if (check->next == NULL)
        return;

    check = check->next;
    while (check->value != -10)
    {
        if (getLine(address, check->value) != line)
        {
            line = getLine(address, check->value);
            sprintf(tmp, "%s: ", address);
            strcat(output, tmp);
            printLine(address, line, output);
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

void grep(char **addresses, int n, char *pattern, bool c, bool l, char *output)
{
    char x[MAX_LINE_LENGTH] = {0};

    if (!c && !l)
    {
        for (int i = 0; i < n; i++)
            printAllLinesGrep(*(addresses + i), pattern, output);
    }

    if (c && !l)
    {
        int count = 0;
        for (int i = 0; i < n; i++)
            count += countAllLinesGrep(*(addresses + i), pattern);
        sprintf(x, "%d\n", count);
        strcat(output, x);
    }

    if (!c && l)
    {
        for (int i = 0; i < n; i++)
        {
            if (countAllLinesGrep(*(addresses + i), pattern))
            {
                sprintf(x, "%s\n", *(addresses + i));
                strcat(output, x);
            }
        }
    }

}

