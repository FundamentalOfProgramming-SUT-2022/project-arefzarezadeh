#include<stdio.h>
#include<stdlib.h>
#include<dirent.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<stdbool.h>

#define MAX_ADDRESS_LENGTH 255
#define MAX_STRING_SIZE 10000

bool isDirectory(char *address)
{
    struct stat fileStat;
    stat(address, &fileStat);
    return !S_ISREG(fileStat.st_mode);
}

void printPreviousBranches(int count, char *str)
{
    char text[4];
    text[0] = 179;
    text[1] = ' ';
    text[2] = ' ';
    text[3] = '\0';
    for (int i = 0; i < count; i++)
        strcat(str, text);
    return;
}

char * printTree(char *address, int depth, int maxDepth, char *output)
{
    if (maxDepth == -1)
        maxDepth = (1 << 30);
    if (depth > maxDepth)
        return output;
    if (depth == 0)
    {
        char tmp[MAX_STRING_SIZE];
        sprintf(tmp, "%s:\n", address);
        strcat(output, tmp);
        printTree(address, 1, maxDepth, output);
        return output;
    }
    DIR *d;
    struct dirent *dir;
    d = opendir(address);
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            if (!strcmp(dir->d_name, ".hidden") || !strcmp(dir->d_name, ".") || !strcmp(dir->d_name, ".."))
            {
                continue;
            }
            printPreviousBranches(depth - 1, output);
            char tmp[MAX_STRING_SIZE];

            sprintf(tmp, "%c%c%c", 195, 196, 196);
            strcat(output, tmp);
            sprintf(tmp, "%s", dir->d_name);
            strcat(output, tmp);

            char newAddress[MAX_ADDRESS_LENGTH] = {0};
            strcat(newAddress, address);
            strcat(newAddress, "/");
            strcat(newAddress, dir->d_name);

            if (isDirectory(newAddress))
            {
                strcat(output, ":\n");
                printTree(newAddress, depth + 1, maxDepth, output);
            }
            else
                strcat(output, "\n");
        }
        closedir(d);
        printPreviousBranches(depth - 1, output);
        char tmp[MAX_STRING_SIZE];
        sprintf(tmp, "%c\n", 223);
        strcat(output, tmp);
    }
    return output;
}
