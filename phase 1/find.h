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

long sizeofFile(FILE *f)
{
    long current = ftell(f);
    fseek(f, 0, SEEK_END);
    long end = ftell(f);
    long size = end - current;
    fseek(f, -size, SEEK_CUR);

    return size;
}

int sizeString(char *text)
{
    int x = 0;
    while (1)
    {
        if (text[x] == '\0' || text[x] == EOF)
            return x;
        x++;
    }
}

char * correctStar(char *text)
{
    char *replacement = (char *) malloc(sizeof(char) * (sizeString(text) + 1));
    int pos = 0;
    for (int i = 0; i < sizeString(text); i++)
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

char * getWord(char *text, int index)
{
    int i = 0;
    int textPos = 0;
    char *word = (char *) malloc(CAPACITY * sizeof(char));
    word[0] = '\0';
    while (1)
    {
        //---------------ignore all the spaces before the word
        while (text[textPos] == ' ' || text[textPos] == '\n')
            textPos++;
        i++;
        //---------------end of that

        //---------------adding the word to the string
        if (i == index)
        {
            int position = 0;
            while (text[textPos] != ' ' && text[textPos] != '\0' && text[textPos] != '\n' && text[textPos] != EOF)
                word[position++] = text[textPos++];
            textPos++;
            word[position] = '\0';
            break;
        }
        //---------------finished adding it

        //---------------ignoring privious words
        while (text[textPos] != ' ' && text[textPos] != '\0' && text[textPos] != '\n' && text[textPos] != EOF)
            textPos++;

        //---------------checking if the string has ended
        if (text[textPos] == EOF || text[textPos] == '\0')
            break;

        textPos++;
    }
    return word;
}

char * fgetword(FILE *r)
{
    //-----------------ignore all spaces before the word
    char c = fgetc(r);
    while (c == ' ' || c == '\n')
        c = fgetc(r);
    //-----------------end of that

    if (c == EOF)
        return NULL;
    char *word = (char *) calloc(CAPACITY, sizeof(char));

    //-----------------get the word
    int i = 0;
    while (c != ' ' && c != '\n' && c != '\0' && c != EOF && i < CAPACITY)
    {
        word[i++] = c;
        c = fgetc(r);
    }
    word[i] = '\0';
    //-----------------end of that

    return word;
}

int fcountWords(FILE *f)
{
    long current = ftell(f);
    int i = 0;
    while(fgetword(f) != NULL)
        i++;
    fseek(f, current, SEEK_SET);

    return i;
}

int countWords(char *text)
{
    int i = 0;
    while (*getWord(text, ++i) != '\0');
    return i - 1;
}

bool wordcmp(char *realWord, char *text)
{
    int size = sizeString(text);
    bool startWild = (text[0] == '*');
    bool endWild = (text[size - 1] == '*' && text[size - 2] != '\\');
    char *correctedText = correctStar(text);
    size = sizeString(correctedText);

    if (!endWild && !startWild) //-------------------------------------case 1
        return !strcmp(realWord, correctedText);

    if (!startWild && endWild) //--------------------------------------case 2
    {
        if (sizeString(realWord) < size - 1)
            return false;
        for (int i = 0; i < size - 1; i++)
        {
            if (realWord[i] != correctedText[i])
                return false;
        }
        return true;
    }

    if (startWild && !endWild) //--------------------------------------case 3
    {
        int wordSize = sizeString(realWord);
        if (wordSize < size - 1)
            return false;
        for (int i = 1; i < size; i++)
        {
            if (realWord[wordSize - i] != correctedText[size - i])
                return false;
        }
        return true;
    }

    if (startWild && endWild) //---------------------------------------case 4
    {
        int wordSize = sizeString(realWord);
        if (wordSize < size - 2)
            return false;
        for (int i = 0; i < wordSize - size + 3; i++)
        {
            bool isSimilar = true;
            for (int pos = 1; pos < size - 1; pos++)
            {
                if (realWord[i + pos - 1] != correctedText[pos])
                {
                    isSimilar = false;
                    break;
                }
            }
            if (isSimilar)
                return true;
        }
        return false;
    }
}

long normalFind(FILE *r, char *textToBeFound, bool byword)
{
    long size = sizeofFile(r);

    if (size <= 0)
        return -1;

    long current = ftell(r);

    int numberOfWords = fcountWords(r);

    int words;

    if (byword)
    {
        fseek(r, 0, SEEK_SET);
        int totalWords = fcountWords(r);
        fseek(r, current, SEEK_SET);
        words = totalWords - numberOfWords;
    }

    for (int i = 1; i <= numberOfWords - countWords(textToBeFound) + 1; i++)
    {
        int wSize = sizeString(fgetword(r));
        fseek(r, -wSize - 1, SEEK_CUR);
        int position = ftell(r);
        words++;
        if (wordcmp(fgetword(r), getWord(textToBeFound, 1)))
        {
            bool check = true;
            for (int j = 2; j <= countWords(textToBeFound); j++)
            {
                if (!wordcmp(fgetword(r), getWord(textToBeFound, j)))
                {
                    check = false;
                    break;
                }
            }

            fseek(r, position, SEEK_SET);
            fgetword(r);

            if (check && !byword)
                return position;
            if (check && byword)
                return words;
        }
    }

    return -1;
}

struct linkedList * find(char *address, char *textToBeFind, int attributes[4])
{
    struct linkedList *output = create_linkedlist();

    FILE *f = fopen(address, "r");

    if (attributes[0])
    {
        int count = 0;
        while (normalFind(f, textToBeFind, false) != -1)
            count++;
        fclose(f);
        addLinkedList(output, count);
        return output;
    }

    if (attributes[1])
    {
        int count = 1;
        while (count++ < attributes[1])
            normalFind(f, textToBeFind, attributes[2]);
        addLinkedList(output, normalFind(f, textToBeFind, attributes[2]));
        fclose(f);
        return output;
    }

    if (attributes[3])
    {
        int answer;
        while (answer = normalFind(f, textToBeFind, attributes[2]) + 1)
            addLinkedList(output, answer - 1);
        fclose(f);
        return output;
    }

    addLinkedList(output, normalFind(f, textToBeFind, attributes[2]));
    fclose(f);
    return output;
}
