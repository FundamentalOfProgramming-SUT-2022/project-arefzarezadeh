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



int main()
{
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
//    printTree(".", 0, -1);

    return 17;
}
