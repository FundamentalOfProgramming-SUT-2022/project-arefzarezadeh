#include<stdio.h>
#include<stdbool.h>

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

int * find(char *address, char *textToBeFound, bool attributes[4])
{
    int size = sizeString(textToBeFound);
    bool startWild = (textToBeFound[0] == '*');
    bool endWild = (textToBeFound[size - 1] == '*' && textToBeFound[size - 2] != '\\');
    printf("%d:%d", startWild, endWild);
}

int main()
{
    createNewFile("test.txt");
    struct pos start = {1, 0};
    insert("test.txt", start, "Salam\nKhobi?");
    struct pos ran = {2, 1};
    cutstr("test.txt", ran, 3, 0);
    ran.line = 1;
    ran.position = 4;
    pastestr("test.txt", ran);

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

    printf("%d\n", wordcmp("hel*lo*", "*lo\\*"));
    printf("%s\n", "hello\\*");
    printf("%s\n", correctStar("hello*"));
    //find("test.txt", "hello*", 0);
    return 17;
}
