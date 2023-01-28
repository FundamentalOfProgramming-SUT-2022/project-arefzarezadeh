#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
//#include<windows.h>

#include "find.h"
#include "grep.h"
#include "tree.h"
#include "others.h"

#define CAPACITY 255
#define COMMAND_LENGTH 10000
#define MAX_STRING_SIZE 10000

/*
    Errors:
    -100: invalid input
    -101: file already exists
    -201: file doesn't exist
    -301: size must be positive
    -302: you forgot to add file
    -303: you forgot to add pos

*/

void errorHandler(int error)
{
    switch (error)
    {
    case -100:
        printf("Invalid input\n");
        break;
    case -101:
        printf("File already exists\n");
        break;
    case -201:
        printf("File doesn't exist\n");
        break;
    case -301:
        printf("Size must be a positive number / You forgot to add size\n");
        break;
    case -302:
        printf("You forgot to add file\n");
        break;
    case -303:
        printf("You forgot to add pos / pos is invalid\n");
        break;
    }
}

char * correctString(char *text)
{
    char *replacement = (char *) malloc(sizeof(char) * (strlen(text) + 1));
    int pos = 0;
    for (int i = 0; i < strlen(text); i++)
    {
        if (text[i] == '\\' && text[i + 1] == 'n')
        {
            replacement[pos] = '\n';
            i++;
        }
        else if (text[i] == '\\' && text[i + 1] == '\\')
        {
            replacement[pos] = '\\';
            i++;
        }
        else if (text[i] == '\\' && text[i + 1] == '\"')
        {
            replacement[pos] = '\"';
            i++;
        }
        else if (text[i] == '\\' && text[i + 1] == 't')
        {
            replacement[pos] = '\t';
            i++;
        }
        else
            replacement[pos] = text[i];
        pos++;
    }
    replacement[pos] = '\0';
    return replacement;
}

char *getString(char *text, int *start)
{
    char *output = (char *) calloc(MAX_STRING_SIZE, sizeof(char));
    int len = 0;
    char c = text[*start];
    *start += 1;
    char previous = 0;
    while (c == ' ')
    {
        c = text[*start];
        *start += 1;
    }
    if (c == '\"')
    {
        c = text[*start];
        *start += 1;
        while (1)
        {
            if (c == '\0' || (c == '"' && previous != '\\'))
                break;
            if (len >= MAX_STRING_SIZE)
                break;
            output[len++] = c;
            previous = c;
            c = text[*start];
            *start += 1;
        }
    }
    else
    {
        while (c != '\0' && c != ' ' && c != '\n')
        {
            if (len >= MAX_STRING_SIZE)
                break;
            output[len++] = c;
            c = text[*start];
            *start += 1;
        }
    }
    return output;
}

int stringToInt(char *num)
{
    int x = 0;
    for (int i = 0; i < strlen(num); i++)
        x = 10 * x + num[i] - '0';
    return x;
}

void setPos(struct pos *p, char *content)
{
    int line = 0;
    int position = 0;
    bool colon = false;
    for (int i = 0; i < strlen(content); i++)
    {
        if (content[i] == ':')
            colon = true;
        else if (!colon)
            line = line * 10 + content[i] - '0';
        else
            position = position * 10 + content[i] - '0';
    }
    p->line = line;
    p->position = position;
}

int terminalCreateNewFile(char *command, int *index)
{
    char *option = getString(command, index);

    if (strcmp(option, "--file"))
    {
        free(option);
        return -100;
    }

    free(option);
    option = getString(command, index);

    int size = strlen(option);
    for (int i = 0; i < size; i++)
        option[i] = option[i + 1];

    if (createNewFile(option))
        return 1;
    else
        return -101;
}

int terminalInsert(char *command, int *index)
{
    char *address;
    char *content;
    struct pos position;
    for (int i = 0; i < 3; i++)
    {
        char *option = getString(command, index);

        bool file = false, str = false, p = false;

        if (!strcmp(option, "--file"))
            file = true;
        else if (!strcmp(option, "--str"))
            str = true;
        else if (!strcmp(option, "--pos"))
            p = true;
        else
        {
            free(option);
            return -100;
        }

        free(option);
        option = getString(command, index);

//        if (!strcmp(option, "--file") || !strcmp(option, "--str") || !strcmp(option, "--pos"))
//        {
//            free(option);
//            return -100;
//        }
        if (*index >= strlen(command) && !strcmp(option, ""))
        {
            free(option);
            return -100;
        }

        if (file)
        {
            int size = strlen(option);
            for (int i = 0; i < size; i++)
                option[i] = option[i + 1];
            address = option;
        }
        else if (str)
            content = option;
        else if (p)
            setPos(&position, option);
    }

    if (!fileExists(address))
        return -201;

    char *trueContent = correctString(content);

    insert(address, position, trueContent);
    return 1;
}

int terminalCat(char *command, int *index)
{
    char *option = getString(command, index);

    if (strcmp(option, "--file"))
    {
        free(option);
        return -100;
    }

    free(option);
    option = getString(command, index);

    int size = strlen(option);
    for (int i = 0; i < size; i++)
        option[i] = option[i + 1];

    if (fileExists(option))
    {
        cat(option);
        return 1;
    }
    else
        return -201;
}

int terminalRemove(char *command, int *index)
{
    char *address = NULL;
    bool forward = true;
    int size = 0;
    struct pos position;
    for (int i = 0; i < 4; i++)
    {
        char *option = getString(command, index);

        bool file = false, siz = false, p = false;

        if (!strcmp(option, "--file"))
            file = true;
        else if (!strcmp(option, "--size"))
            siz = true;
        else if (!strcmp(option, "--pos"))
            p = true;
        else if (!strcmp(option, "-b"))
        {
            forward = false;
            continue;
        }
        else if (!strcmp(option, "-f"))
        {
            forward = true;
            continue;
        }
        else if (!strcmp(option, ""))
            break;
        else
        {
            free(option);
            return -100;
        }

        free(option);
        option = getString(command, index);

        if (file)
        {
            int fsize = strlen(option);
            for (int i = 0; i < fsize; i++)
                option[i] = option[i + 1];
            address = option;
        }
        else if (siz)
            size = stringToInt(option);
        else if (p)
            setPos(&position, option);
    }

    if (!fileExists(address))
        return -201;
    if (size <= 0)
        return -301;
    if (address == NULL)
        return -302;
    if (position.line == -1)
        return -303;

    removestr(address, position, size, forward);
    return 1;
}

int terminalCopy(char *command, int *index)
{
    char *address = NULL;
    bool forward = true;
    int size = 0;
    struct pos position = {-1, 0};
    for (int i = 0; i < 4; i++)
    {
        char *option = getString(command, index);

        bool file = false, siz = false, p = false;

        if (!strcmp(option, "--file"))
            file = true;
        else if (!strcmp(option, "--size"))
            siz = true;
        else if (!strcmp(option, "--pos"))
            p = true;
        else if (!strcmp(option, "-b"))
        {
            forward = false;
            continue;
        }
        else if (!strcmp(option, "-f"))
        {
            forward = true;
            continue;
        }
        else if (!strcmp(option, ""))
            break;
        else
        {
            free(option);
            return -100;
        }

        free(option);
        option = getString(command, index);

        if (file)
        {
            int fsize = strlen(option);
            for (int i = 0; i < fsize; i++)
                option[i] = option[i + 1];
            address = option;
        }
        else if (siz)
            size = stringToInt(option);
        else if (p)
            setPos(&position, option);
    }

    if (!fileExists(address))
        return -201;
    if (size <= 0)
        return -301;
    if (address == NULL)
        return -302;
    if (position.line == -1)
        return -303;

    copystr(address, position, size, forward);
    return 1;
}

int terminalCut(char *command, int *index)
{
    char *address = NULL;
    bool forward = true;
    int size = 0;
    struct pos position;
    for (int i = 0; i < 4; i++)
    {
        char *option = getString(command, index);

        bool file = false, siz = false, p = false;

        if (!strcmp(option, "--file"))
            file = true;
        else if (!strcmp(option, "--size"))
            siz = true;
        else if (!strcmp(option, "--pos"))
            p = true;
        else if (!strcmp(option, "-b"))
        {
            forward = false;
            continue;
        }
        else if (!strcmp(option, "-f"))
        {
            forward = true;
            continue;
        }
        else if (!strcmp(option, ""))
            break;
        else
        {
            free(option);
            return -100;
        }

        free(option);
        option = getString(command, index);

        if (file)
        {
            int fsize = strlen(option);
            for (int i = 0; i < fsize; i++)
                option[i] = option[i + 1];
            address = option;
        }
        else if (siz)
            size = stringToInt(option);
        else if (p)
            setPos(&position, option);
    }

    if (!fileExists(address))
        return -201;
    if (size <= 0)
        return -301;
    if (address == NULL)
        return -302;
    if (position.line == -1)
        return -303;

    cutstr(address, position, size, forward);
    return 1;
}

int terminalPaste(char *command, int *index)
{
    char *address = NULL;
    struct pos position = {-1, 0};
    for (int i = 0; i < 4; i++)
    {
        char *option = getString(command, index);

        bool file = false, p = false;

        if (!strcmp(option, "--file"))
            file = true;
        else if (!strcmp(option, "--pos"))
            p = true;
        else if (!strcmp(option, ""))
            break;
        else
        {
            free(option);
            return -100;
        }

        free(option);
        option = getString(command, index);

        if (file)
        {
            int fsize = strlen(option);
            for (int i = 0; i < fsize; i++)
                option[i] = option[i + 1];
            address = option;
        }
        else if (p)
            setPos(&position, option);
    }

    if (!fileExists(address))
        return -201;
    if (address == NULL)
        return -302;
    if (position.line == -1)
        return -303;

    pastestr(address, position);
    return 1;
}

bool terminal()
{
    char command[COMMAND_LENGTH];
    scanf("%[^\n]", command);
    getchar();

    int index = 0;
    char *first = getString(command, &index);

    if (!strcmp(first, "createfile"))
    {
        int output = terminalCreateNewFile(command, &index);
        if (output != 1)
            errorHandler(output);

        return true;
    }
    else if (!strcmp(first, "insertstr"))
    {
        int output = terminalInsert(command, &index);
        if (output != 1)
            errorHandler(output);

        return true;
    }
    else if (!strcmp(first, "cat"))
    {
        int output = terminalCat(command, &index);
        if (output != 1)
            errorHandler(output);

        return true;
    }
    else if (!strcmp(first, "removestr"))
    {
        int output = terminalRemove(command, &index);
        if (output != 1)
            errorHandler(output);

        return true;
    }
    else if (!strcmp(first, "copystr"))
    {
        int output = terminalCopy(command, &index);
        if (output != 1)
            errorHandler(output);

        return true;
    }
    else if (!strcmp(first, "cutstr"))
    {
        int output = terminalCut(command, &index);
        if (output != 1)
            errorHandler(output);

        return true;
    }
    else if (!strcmp(first, "pastestr"))
    {
        int output = terminalPaste(command, &index);
        if (output != 1)
            errorHandler(output);

        return true;
    }

    printf("Invalid expression\n");
    return true;
}

int main()
{
    while (terminal());
    //createNewFile("test.txt");
    //createNewFile("text.txt");
    struct pos start = {1, 0};
    //int att[4] = {0, 0, 0, 0};
//    struct pos found1 = getWordPosFromIndex("test.txt", find("test.txt", "K*", att)->value);
//    struct pos found2 = getWordPosFromIndex("test.txt", find("test.txt", "Kh\\*obi?", att)->value);
//    printf("line:%d\n", found1.line);
//    printf("pos:%d\n", found1.position);
    int att[2] = {0, 1};
    replace("test.txt", "K*", "|locate|", att);
//    FILE *read = fopen("test.txt", "r");
//    printf("%d\n", findNormal(read, "K* c"));


    //insert("test.txt", start, "b b b b Salam\nKhobi?\nchert Khobi? chert a a b chert2 a chert chert3");
    //insert("text.txt", start, "{{}            }");
    //auto_indent("text.txt");
//    FILE *r = fopen("text.txt", "r");
//    FILE *r2 = fopen("test.txt", "r");
//
//    struct pos middle = {1, 4};
//    char text1[100];
//    char text2[100];
//
//    //insert("test.txt", middle, "xlolololx");
//    fgets(text1, 100, r);
//    fgets(text2, 100, r2);
//
//    printf("%d\n", strcmp(text1, text2));
//
//    fgets(text1, 100, r);
//    fgets(text2, 100, r2);
//
//    printf("%d\n", strcmp(text1, text2));
//
//    fgets(text1, 100, r);
//    fgets(text2, 100, r2);
//
//    printf("%d\n", strcmp(text1, text2));
//    printf("%s/%s/%d/%d\n", text1, text2, ftell(r), ftell(r2));
//
//    fgets(text1, 100, r);
//    fgets(text2, 100, r2);
//
//    printf("%d\n", strcmp(text1, text2));
//    printf("%s/%s/%d/%d\n", text1, text2, ftell(r), ftell(r2));
//
//    fgets(text1, 100, r);
//    fgets(text2, 100, r2);
//
//    printf("%d\n", strcmp(text1, text2));
//    printf("%s/%s/%d/%d/%d\n", text1, text2, ftell(r), ftell(r2), strlen(text2));
//    fclose(r);
//    fclose(r2);
//
//    textComparator("text.txt", "test.txt");
//
//
//
//    undo();
//    undo();
//
    printTree(".", 0, -1);

    return 17;
}
