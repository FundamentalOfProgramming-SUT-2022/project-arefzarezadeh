#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>

#include "find.h"

#define CAPACITY 255

struct pos
{
    int line;
    int position;
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

//int sizeString(char *text)
//{
//    int x = 0;
//    while (1)
//    {
//        if (text[x] == '\0' || text[x] == EOF)
//            return x;
//        x++;
//    }
//}

//char * getWord(char *text, int index)
//{
//    int i = 1;
//    int textPos = 0;
//    //while (text[textPos] == ' ' && text[textPos] == '\n')
//    char *word = (char *) malloc(CAPACITY * sizeof(char));
//    while (1)
//    {
//        if (i == index)
//        {
//            int position = 0;
//            while (text[textPos] != ' ' && text[textPos] != '\0' && text[textPos] != '\n' && text[textPos] != EOF)
//                word[position++] = text[textPos++];
//            textPos++;
//            word[position] = '\0';
//            break;
//        }
//
//        while (text[textPos] != ' ' && text[textPos] != '\0' && text[textPos] != '\n' && text[textPos] != EOF)
//            textPos++;
//        textPos++;
//        i++;
//    }
//    return word;
//}

//char * fgetword(FILE *r, int cap)
//{
//    char *word = (char *) malloc(cap * sizeof(char));
//    char c = fgetc(r);
//    int pos = 0;
//    while (c != ' ' && c != '\n' && c != '\0' && c != EOF && pos < cap)
//    {
//        word[pos++] = c;
//        c = fgetc(r);
//    }
//    word[pos] = '\0';
//    return word;
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

//char * correctStar(char *text)
//{
//    char *replacement = (char *) malloc(sizeof(char) * (sizeString(text) + 1));
//    int pos = 0;
//    for (int i = 0; i < sizeString(text); i++)
//    {
//        if (text[i] == '\\' && text[i + 1] == '*')
//        {
//            replacement[pos] = '*';
//            i++;
//        }
//        else
//            replacement[pos] = text[i];
//        pos++;
//    }
//    replacement[pos] = '\0';
//    return replacement;
//}

//bool wordcmp(char *realWord, char *text)
//{
//    int size = sizeString(text);
//    bool startWild = (text[0] == '*');
//    bool endWild = (text[size - 1] == '*' && text[size - 2] != '\\');
//    char *correctedText = correctStar(text);
//    size = sizeString(correctedText);
//
//    if (!endWild && !startWild) //-------------------------------------case 1
//        return !strcmp(realWord, correctedText);
//
//    if (!startWild && endWild) //--------------------------------------case 2
//    {
//        if (sizeString(realWord) < size - 1)
//            return false;
//        for (int i = 0; i < size - 1; i++)
//        {
//            if (realWord[i] != correctedText[i])
//                return false;
//        }
//        return true;
//    }
//
//    if (startWild && !endWild) //--------------------------------------case 3
//    {
//        int wordSize = sizeString(realWord);
//        if (wordSize < size - 1)
//            return false;
//        for (int i = 1; i < size; i++)
//        {
//            if (realWord[wordSize - i] != correctedText[size - i])
//                return false;
//        }
//        return true;
//    }
//
//    if (startWild && endWild) //---------------------------------------case 4
//    {
//        int wordSize = sizeString(realWord);
//        if (wordSize < size - 2)
//            return false;
//        for (int i = 0; i < wordSize - size + 3; i++)
//        {
//            bool isSimilar = true;
//            for (int pos = 1; pos < size - 1; pos++)
//            {
//                if (realWord[i + pos - 1] != correctedText[pos])
//                {
//                    isSimilar = false;
//                    break;
//                }
//            }
//            if (isSimilar)
//                return true;
//        }
//        return false;
//    }
//}

//int normalFind(char *address, char *textToBeFound, int from, bool countFirstWord)
//{
//    int foundWords = 0;
//    int foundChars = 0;
//    int charNum = 0;
//    int setPos = 0;
//    FILE *r = fopen(address, "r");
//    fseek(r, 0, SEEK_END);
//    int size = ftell(r) - from;
//    fseek(r, 0, SEEK_SET);
//
//    bool found = false;
//
//    for (int i = 0; i < from; i++)
//        fgetc(r);
//
//    while (charNum < size)
//    {
//        char *tmp = fgetword(r, CAPACITY);
//        if (wordcmp(tmp, getWord(textToBeFound, foundWords + 1)))
//            foundChars += sizeString(getWord(textToBeFound, ++foundWords)) + 1;
//        else
//            foundChars = foundWords = 0;
//        charNum += sizeString(tmp) + 1;
//        if (!foundWords)
//            setPos = charNum;
//        if (foundChars >= sizeString(textToBeFound))
//        {
//            if (!(!countFirstWord && setPos == 0))
//            {
//                found = true;
//                break;
//            }
//        }
//    }
//    fclose(r);
//    if (found)
//        return setPos;
//    else
//        return -1;
//}

//int findByWord(char *address, char *textToBeFound, int from, bool countFirstWord)
//{
//    int foundWords = 0;
//    int foundChars = 0;
//    int charNum = 0;
//    int wordNum = 1;
//    int setPos = 1;
//    FILE *r = fopen(address, "r");
//    fseek(r, 0, SEEK_END);
//    int size = ftell(r) - from;
//    fseek(r, 0, SEEK_SET);
//
//    bool found = false;
//
//    for (int i = 0; i < from; i++)
//        fgetword(r, CAPACITY);
//
//    while (charNum < size)
//    {
//        char c = fgetc(r);
//        while (c == ' ')
//            c = fgetc(r);
//        fseek(r, -1, SEEK_CUR);
//        char *tmp = fgetword(r, CAPACITY);
//        if (wordcmp(tmp, getWord(textToBeFound, foundWords + 1)))
//            foundChars += sizeString(getWord(textToBeFound, ++foundWords)) + 1;
//        else
//            foundChars = foundWords = 0;
//        charNum += sizeString(tmp) + 1;
//        wordNum++;
//        if (!foundWords)
//            setPos = wordNum;
//        if (foundChars >= sizeString(textToBeFound))
//        {
//            if (countFirstWord || setPos != 1)
//            {
//                found = true;
//                break;
//            }
//            else if (setPos == 1)
//                setPos++;
//        }
//    }
//    fclose(r);
//    if (found)
//        return setPos;
//    else
//        return -1;
//}

//int findByWord(char *address, )

//int * find(char *address, char *textToBeFound, int from, int attributes[4])
//{
//    //printf("//opened find\\\\\n");
//
//    int *output;
//    output = (int *)calloc(1, sizeof(int));
//
//    if (!attributes[0] && !attributes[1] && !attributes[2] && !attributes[3])//normal find
//        *output = normalFind(address, textToBeFound, 0, true);
//    else if (!attributes[0] && !attributes[1] && attributes[2] && !attributes[3])//find by word
//        *output = findByWord(address, textToBeFound, 0, true);
//    else if (attributes[0] && !attributes[1] && !attributes[2] && !attributes[3])//count
//    {
//        int x = normalFind(address, textToBeFound, from, !from);
//        if (x == -1)
//            *output = 0;
//        else
//            *output = 1 + *find(address, textToBeFound, x + 1 + from, attributes);
//    }//--------------end of count
//    else if (!attributes[0] && attributes[1] && !attributes[2] && !attributes[3])//find at
//    {
//        int nfrom = from;
//        int x;
//        for (int i = 0; i < attributes[1]; i++)
//        {
//            x = normalFind(address, textToBeFound, nfrom, !nfrom);
//            if (x != -1)
//                nfrom = x + 1 + nfrom;
//            else
//            {
//                nfrom = 0;
//                break;
//            }
//        }
//        *output = nfrom - 1;
//    }//-------end of find at
//    else if (!attributes[0] && attributes[1] && attributes[2] && !attributes[3])//find at by word
//    {
//        int nfrom = from;
//        int x;
//        for (int i = 0; i < attributes[1]; i++)
//        {
//            x = findByWord(address, textToBeFound, nfrom, 0);
//            printf("//%d\\\\\n", x);
//            if (x != -1)
//                nfrom += x;
//            else
//            {
//                nfrom = 0;
//                break;
//            }
//        }
//        *output = nfrom;
//    }//------end of find at by word
//    else if (!attributes[0] && !attributes[1] && !attributes[2] && attributes[3])//find all
//    {
//        int x = normalFind(address, textToBeFound, from, true);
//        {
//
//        }
//    }
//    return output;
//}

int main()
{
    createNewFile("test.txt");
    struct pos start = {1, 0};

    //struct pos ran = {2, 1};
    //cutstr("test.txt", ran, 3, 0);
    //ran.line = 1;
    //ran.position = 4;
    //pastestr("test.txt", ran);

    /*char *text = (char *) malloc(10 * sizeof(char));
    char c = getchar();
    int i = 0;
    printf("%d\n", i);
    while (c != EOF && c != '\n')
    {
        text[i] = c;
        i++;
        c = getchar();
        printf("%d\n", i);

    }
    text[i] = '\0';
    printf("%s\n%d\n\n", text, strcmp(text, "hello\\*"));*/

//    printf("%d\n", wordcmp("hel*lo*", "*lo\\*"));
//    printf("%s\n", "hello\\*");

    FILE *x = fopen("test.txt", "r");
//    char *c = getWord("*lam K*", 1);
//    printf("%s:-->%d\n", c, sizeString(c));
//    c = getWord("*lam K*", 2);
//    printf("%s:-->%d\n", c, sizeString(c));
//    c = fgetword(x, CAPACITY);
//    printf("%s:-->%d\n", c, sizeString(c));
//    c = fgetword(x, CAPACITY);
//    printf("%s:-->%d\n", c, sizeString(c));

    int att[4] = {0, 0, 1, 1};
//    for (int i = 1; i <= 15; i++)
//        printf("%s\n", getWord("b b b b Salam\nKhobi?\nchert Khobi? chert a a a chert2 chert chert3", i));
    insert("test.txt", start, "b b b b Salam\nKhobi?\nchert Khobi? chert a a chert2 a chert chert3");
    //printf(",,%d,,\n", findByWord("test.txt", "b", 1, true));
    printLinkedList(find("test.txt", "b", att));

    //find("test.txt", "hello*", 0);
    return 17;
}
