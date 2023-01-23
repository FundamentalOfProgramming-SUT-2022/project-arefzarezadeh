#include<stdio.h>
#include<stdlib.h>
#include<dirent.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<stdbool.h>

#define MAX_ADDRESS_LENGTH 255

bool isDirectory(char *address)
{
    struct stat fileStat;
    stat(address, &fileStat);
    return !S_ISREG(fileStat.st_mode);
}

int countSubDir(char *address)
{
    DIR *d;
    struct dirent *dir;
    d = opendir(address);

    int count = 0;
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
            count++;
        closedir(d);
    }

    return count;
}

void printPreviousBranches(int count)
{
    for (int i = 0; i < count; i++)
        printf("%c  ", 179);
    return;
}

void printTree(char *address, int depth, int maxDepth)
{
    if (maxDepth == -1)
        maxDepth = (1 << 30);
    if (depth > maxDepth)
        return;
    if (depth == 0)
    {
        printf("%s:\n", address);
        printTree(address, 1, maxDepth);
        return;
    }
    DIR *d;
    struct dirent *dir;
    d = opendir(address);
    if (d)
    {
        int count = countSubDir(address);
        int i = 0;
        while ((dir = readdir(d)) != NULL)
        {
            i++;
            if (!strcmp(dir->d_name, ".hidden") || !strcmp(dir->d_name, ".") || !strcmp(dir->d_name, ".."))
            {
                count--;
                i--;
                continue;
            }
            printPreviousBranches(depth - 1);
            if (i == count)
                printf("%c%c%c", 192, 196, 196);
            else
                printf("%c%c%c", 195, 196, 196);
            printf("%s", dir->d_name);

            char newAddress[MAX_ADDRESS_LENGTH] = {0};
            strcat(newAddress, address);
            strcat(newAddress, "/");
            strcat(newAddress, dir->d_name);

            if (isDirectory(newAddress))
            {
                printf(":\n");
                printTree(newAddress, depth + 1, maxDepth);
            }
            else
                printf("\n");
        }
        closedir(d);
    }
}
