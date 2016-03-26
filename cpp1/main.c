#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define MAX_STR 1024

int detect(char * str){
    int idetect=0;
    if (*str =='\n')
        return 1;
    while (*str != '\0'){
        if(idetect<0)
            return 1;
        if(*str=='('){
            idetect++;
            str++;
            continue;
        }
        if(*str==')'){
            idetect--;
            str++;
            continue;
        }
        str++;
    }
    if(idetect)
        return 1;
    return 0;
}

int main() {
    char str[MAX_STR];
    char **array=NULL;
    array = (char**)malloc(sizeof(char*));
    if(array == NULL) {
        printf("%s","[error]");
        return 0;
    }
    int count=1;
    //int count=0;
    char *str1 = NULL;
    char **buffer = NULL;
    while (fgets(str, MAX_STR, stdin) != NULL){
        str1 = (char*)calloc((strlen(str) + 1) , sizeof(char));
        if(str1 == NULL) {
            printf("%s","[error]");
            for (int i = 0; i < count - 1; ++i)
                free(array);
            return 0;
        }
        strcpy(str1, str);
        str1[strlen(str)]='\0';
        array[count-1] = str1;
        if( (buffer = (char**)realloc(array, sizeof(char*) * (++count))) == NULL){
            printf("%s","[error]");
            for (int i = 0; i < count - 1; ++i) {
                free(array[i]);
            }
            free(str1);
            return 0;
        } else {
            array = buffer;
        }
    }
    int res_of_detect=0;
    for (int i = 0; i <count - 1; ++i) {
        if(!detect(array[i])){
            res_of_detect++;
            printf("%s",array[i]);
        }
    }
    for (int i = 0; i <count - 1; ++i) {
        free(array[i]);
    }
    free(array);
    return 0;
}