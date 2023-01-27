#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>
#include<string.h>

#define CAPACITY 255

struct linkedList
{
    int value;
    struct linkedList *next;
};

struct linkedList* create_linkedlist()
{
    struct linkedList *first = (struct linkedList *) malloc(sizeof(struct linkedList));
    first->value = -10;
    first->next = NULL;
    return first;
}

void addLinkedList(struct linkedList* l, int val)
{
    struct linkedList* current = l;
    while (current->value != -10)
    {
        if (current->next == NULL)
        {
            current->next = create_linkedlist();
            current = current->next;
        }
        else
            current = current->next;
    }
    current->value = val;
    return;
}

void printLinkedList(struct linkedList *list)
{
    struct linkedList *check = list;
    if (check->value == -10)
        return;
    printf("%d", check->value);
    if (check->next == NULL)
        return;
    check = check->next;
    while (check->value != -10)
    {
        printf(", %d", check->value);
        if (check->next == NULL)
            break;
        check = check->next;
    }
    printf("\n");
    return;
}

int getRealPos(FILE *file)
{
    long position = ftell(file);
    if (position <= 0)
        return position;
    fseek(file, 0, SEEK_SET);
    int ans = 0;
    int told = 0;
    while (told != position)
    {
        fgetc(file);
        told = ftell(file);
        ans++;
    }
    return ans;
}

char * correctStar(char *text)
{
    char *replacement = (char *) malloc(sizeof(char) * (strlen(text) + 1));
    int pos = 0;
    for (int i = 0; i < strlen(text); i++)
    {
        if (text[i] == '\\' && text[i + 1] == '*')
        {
            replacement[pos] = '*';
            i++;
        }
        else
            replacement[pos] = text[i];
        pos++;
    }
    replacement[pos] = '\0';
    return replacement;
}

int staticFind(FILE *read, char *textToBeFound)
{
    char *newText = correctStar(textToBeFound);
    long output = -1;
    while (1)
    {
        char c = fgetc(read);
        output++;
        if (c == EOF)
            break;

        if (c == newText[0])
        {
            long position = ftell(read);
            bool check = true;
            for (int i = 1; i < strlen(newText); i++)
            {
                c = fgetc(read);
                if (c != newText[i])
                {
                    check = false;
                    break;
                }
            }

            if (check)
                return output;
            fseek(read, position, SEEK_SET);
        }
    }

    return -1;
}

int firstSpace(FILE *read)
{
    long position = ftell(read);
    long output = 0;
    while (1)
    {
        char c = fgetc(read);
        if (c == ' ' || c == '\n' || c == '\t' || c == '\0' || c == EOF)
        {
            fseek(read, position, SEEK_SET);
            return output;
        }
        output++;
    }
}

int lastSpace(FILE *read)
{
    long position = ftell(read);
    if (position <= 0)
        return 0;
    fseek(read, -1, SEEK_CUR);

    int output = 0;
    while (1)
    {
        char c = fgetc(read);
        if (ftell(read) == 0 || c == ' ')
        {
            fseek(read, position, SEEK_SET);
            return output;
        }
        else if (c == '\n')
        {
            fseek(read, position, SEEK_SET);
            return output;
        }
        fseek(read, -2, SEEK_CUR);
        output++;
    }
}

int findStartWild(FILE *read, char *textToBeFound)
{
    int textSize = strlen(textToBeFound);
    char *newText = (char *) malloc(textSize * sizeof(char));
    int backN = false;
    for (int i = 0; i < textSize; i++)
    {
        newText[i] = textToBeFound[i + 1];
        if (newText[i] == '\n')
            backN++;
    }

    long firstSeen = staticFind(read, newText);
    if (firstSeen == -1)
        return -1;
    long firstPos = ftell(read);
    long firstSpaceSeen = firstSpace(read);
    fseek(read, -strlen(newText), SEEK_CUR);
    long lastSpaceSeen = lastSpace(read);
    fseek(read, strlen(newText), SEEK_CUR);
    while (1)
    {
        long secondSeen = staticFind(read, newText);
        if (secondSeen == -1 || secondSeen > firstSpaceSeen)
        {
            fseek(read, firstPos, SEEK_SET);
            free(newText);
            return firstSeen - lastSpaceSeen + backN;
        }
        else
        {
            firstSeen += secondSeen + 1;
            firstPos = ftell(read);
            firstSpaceSeen -= secondSeen + 1;
            lastSpaceSeen += secondSeen + 1;
        }
    }
}

int findEndWild(FILE *read, char *textToBeFound)
{
    int textSize = strlen(textToBeFound);
    char *newText = (char *) malloc(textSize * sizeof(char));
    for (int i = 0; i < textSize - 1; i++)
        newText[i] = textToBeFound[i];
    newText[textSize - 1] = '\0';

    int output = staticFind(read, newText);
    if (output == -1)
        return -1;
    free(newText);
    fseek(read, firstSpace(read), SEEK_CUR);
    return output;
}

int findBothWild(FILE *read, char *textToBeFound)
{
    int textSize = strlen(textToBeFound);
    char *newText = (char *) malloc(textSize * sizeof(char));
    for (int i = 0; i < textSize - 1; i++)
        newText[i] = textToBeFound[i];
    newText[textSize - 1] = '\0';

    int output = findStartWild(read, newText);
    fseek(read, firstSpace(read), SEEK_CUR);
    return output;
}

int findNormal(FILE *read, char *textToBeFound)
{
    int textLength = strlen(textToBeFound);
    int count_star = 0;
    for (int i = 0; i < textLength; i++)
    {
        if ((textToBeFound[i] == '*' && i == 0) || (textToBeFound[i] == '*' && textToBeFound[i - 1] != '\\'))
            count_star++;
    }

    if (count_star == 0)
        return staticFind(read, textToBeFound);

    if (textToBeFound[0] == '*' && count_star == 1)
        return findStartWild(read, textToBeFound);

    if (textToBeFound[textLength - 1] == '*' && count_star == 1)
        return findEndWild(read, textToBeFound);

    if (textToBeFound[textLength - 1] == '*' && textToBeFound[0] == '*' && count_star == 2)
        return findBothWild(read, textToBeFound);

    if (textToBeFound[0] == '*')
    {
        char *firstWord = (char *) calloc(textLength, sizeof(char));
        int i;
        for (i = 0; i < textLength; i++)
        {
            firstWord[i] = textToBeFound[i];
            if (firstWord[i] == ' ' || firstWord[i] == '\n' || firstWord[i] == '\0' || firstWord[i] == '\t' || firstWord[i] == EOF)
                break;
            if (i > 0 && firstWord[i] == '*' && firstWord[i - 1] != '\\')
                break;
        }
        char *rest = (char *) calloc(textLength, sizeof(char));
        for (int j = 1; j < textLength - i; j++)
            rest[j - 1] = textToBeFound[j + i];

//        if (!strcmp(rest, ""))
//            return findStartWild(read, firstWord);
        while (1)
        {
            int output;
            if (i > 0 && firstWord[i] == '*' && firstWord[i - 1] != '\\')
                output = findBothWild(read, firstWord);
            else
                output = findStartWild(read, firstWord);
            long position = ftell(read);
            int secondPart = findNormal(read, rest);
            if (output == -1 || secondPart == 0)
            {
                free(rest);
                free(firstWord);
                return output;
            }
            fseek(read, position, SEEK_SET);
        }
    }

    char *first = (char *) calloc(textLength, sizeof(char));
    char *rest = (char *) calloc(textLength, sizeof(char));

    int i;

    for (i = 0; i < textLength; i++)
    {
        first[i] = textToBeFound[i];
        if (first[i] == '*' && first[i - 1] != '\\')
        {
            if (first[i - 1] == ' ' || first[i - 1] == '\n'|| first[i - 1] == EOF|| first[i - 1] == '\0')
                first[i--] = '\0';
            break;
        }
    }

    for (int j = 1; j < textLength - i; j++)
        rest[j - 1] = textToBeFound[j + i];

    //printf("first:%s|rest:%s\n", first, rest);

    if (!strcmp(rest, ""))
        return findNormal(read, first);
    while (1)
    {
        int initPos = getRealPos(read);
        int output = findNormal(read, first);
        long position = ftell(read);
        int secondPart = findNormal(read, rest);
        //printf("%s:%d|%s:%d\n", first, output, rest, secondPart);
        if (output == -1 || secondPart == 0)
        {
            free(rest);
            free(first);
            if (output == -1)
                return -1;
            return output + initPos;
        }
        fseek(read, position, SEEK_SET);
    }
}

struct linkedList * find(char *address, char *textToBeFound, int attributes[4])
{
    struct linkedList *output = create_linkedlist();

    FILE *read = fopen(address, "r");

    if (attributes[0])
    {
        int count = 0;
        while (findNormal(read, textToBeFound) != -1)
            count++;
        fclose(read);
        addLinkedList(output, count);
        return output;
    }

    if (attributes[1])
    {
        int count = 0;
        int answer = 0;
        while (count++ < attributes[1])
        {
            int tmp2 = getRealPos(read);
            int tmp = findNormal(read, textToBeFound) + 1;
            if (tmp)
                answer = tmp2 + tmp;
            else
            {
                addLinkedList(output, -1);
                fclose(read);
                return output;
            }
        }
        addLinkedList(output, answer - 1);
        fclose(read);
        return output;
    }

    if (attributes[3])
    {
        int count = 0;
        int answer = 0;
        while (1)
        {
            int tmp2 = getRealPos(read);
            int tmp = findNormal(read, textToBeFound) + 1;
            if (tmp)
            {
                answer = tmp2 + tmp;
                addLinkedList(output, answer - 1);
            }
            else
            {
                fclose(read);
                return output;
            }
        }
    }

    addLinkedList(output, findNormal(read, textToBeFound));
    fclose(read);
    return output;

//    addLinkedList(output, findNormal(read, textToBeFound, 0));
//    addLinkedList(output, findNormal(read, textToBeFound, 0));
//    fclose(read);
//    return output;
}



//int main()
//{
//    FILE *r = fopen("text.txt", "r");
//
//    //fseek(r, 10, SEEK_SET);
//
////    fseek(r, 39, SEEK_SET);
////    printf("%c%", fgetc(r));
////    printf("%c%", fgetc(r));
////    printf("%c%", fgetc(r));
////    printf("%c%", fgetc(r));
//
////    printf("find *l:%d\n", findStartWild(r, "*obi?"));
////    printf("find *l:%d\n", staticFind(r, "chert"));
//
////    int output = findStartWild(r, "*?\n");
////    long position = ftell(r);
////    int secondPart = staticFind(r, "chert");
////    printf("output:%d\nsecond:%d\n\n", output, secondPart);
//
//
//    int att[4] = {0, 0, 0, 1};
//    struct linkedList *list = find("test.txt", "K", att);
//    printLinkedList(list);
////    printf("find:%d\n", find("test.txt", "b*", att)->value);
////    printf("find *l:%d\n", findNormal(r, "*? *e"));
////    printf("find *l:%d\n", findNormal(r, "*? *e"));
////    printf("find *l:%d\n", findNormal(r, "*? *e"));
//    //printf("find *l:%d\n", find("test.txt", "? *t", att)->next->value);
//    //printf("find *l:%d\n", findNormal(r, "*lam\nKhobi?\nc"));
////    printf("find *l:%d\n", findNormal(r, "*hert"));
//    //printf("find *l:%d\n", findNormal(r, "chert *? c"));
//    //printf("find *l:%d\n", findNormal(r, "*? c"));
//    //printf("find *l:%d\n", findNormal(r, "c"));
//    printf("%c%", fgetc(r));
//    printf("%c%", fgetc(r));
//    printf("%c%", fgetc(r));
//    printf("%c%", fgetc(r));
//    printf("%c%", fgetc(r));
//    printf("%c%", fgetc(r));
//    printf("%c%", fgetc(r));
//    //printf("find *l:%d\n", staticFind(r, "am"));
////    printf("%d\n", staticFind(r, "b"));
////    printf("%d\n", staticFind(r, "b"));
////    printf("%d\n", staticFind(r, "b"));
////    printf("%d\n", staticFind(r, "b"));
////    printf("%d\n", staticFind(r, "b"));
////    printf("%d\n", staticFind(r, "b"));
////    printf("%d\n", staticFind(r, "b"));
////    printf("%d\n", staticFind(r, "b"));
////    printf("%d\n", staticFind(r, "b"));
////    printf("%d\n", staticFind(r, "b"));
////    printf("%d\n", staticFind(r, "b"));
////    printf("%d\n", staticFind(r, "b"));
////    printf("%d\n", ftell(r));
//
////    fseek(r, 0, SEEK_SET);
////    char c;
////    while ((c = fgetc(r)) != EOF)
////        printf("%d:'%c'\n", ftell(r) - 1, c);
//    return 17;
//}
