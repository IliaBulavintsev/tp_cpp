//Требуется написать программу, которая способна вычислять арифметические выражения.
//Выражения могут содержать:
//1) знаки операций '+', '-', '/', '*'
//2) Скобки '(', ')'
//3) Целые и вещественные числа, в нотации '123', '123.345', все операции должны быть вещественны, а результаты выведены с точностю до двух знаков после запятой в том числе целые '2.00'
//4) необходимо учитывать приоритеты операций, и возможность унарного минуса, пробелы ничего не значат
//5) Если в выражении встретилась ошибка требуется вывести в стандартный поток вывода "[error]" (без кавычек)

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#define MAX_STR 1024

typedef struct Stack {
    char *symbol;
    size_t size;
    int real_size;
} Stack;

typedef struct D_Stack {
    double *symbol;
    size_t size;
    int real_size;
} D_Stack;

Stack* Stack_Init (Stack *stack, size_t n){
    stack = (Stack *)calloc(1, sizeof(Stack));
    stack->symbol = (char*)calloc(n, sizeof(char));
    stack->size = n;
    stack->real_size = 0;
    return stack;
}
D_Stack* D_Stack_Init (D_Stack *stack, size_t n){
    stack = (D_Stack *)calloc(1, sizeof(D_Stack));
    stack->symbol = (double*)calloc(n, sizeof(double));
    stack->size = n;
    stack->real_size = 0;
    return stack;
}
void Stack_Free(Stack *stack){
    free(stack->symbol);
    free(stack);
}
void D_Stack_Free(D_Stack *stack){
    free(stack->symbol);
    free(stack);
}
void Stack_Push(Stack *stack,char op){
    assert(stack->real_size <= stack->size && stack->real_size >= 0);
    stack->symbol[stack->real_size] = op;
    stack->real_size++;
};
void D_Stack_Push(D_Stack *stack,double op){
    assert(stack->real_size <= stack->size && stack->real_size >= 0);
    stack->symbol[stack->real_size] = op;
    stack->real_size++;
};
char Stack_Pop(Stack *stack){
    assert(stack->real_size > 0);
    stack->real_size--;
    return stack->symbol[stack->real_size];
}
double D_Stack_Pop(D_Stack *stack){
    assert(stack->real_size > 0);
    stack->real_size--;
    return stack->symbol[stack->real_size];
}

int operation_prior(char op){
    if (op == '+' || op == '-' ){
        return 2;
    } else if (op == '*' || op == '/'){
        return 4;
    } else if (op == ')'){
        return 6;
    }
    else if (op == '('){
        return 1;
    }
    return 0;
}

int is_operation(char op){
    if (op == '+' || op == '-' || op == '*' || op == '/' || op == ')' || op == '('){
        return 1;
    } else  if (op > 47 && op < 58){
        return 0;
    } else {
        return -1;
    }
}

double create_double(char *str, int *iter){
    double result = 0;
    char *buffer_to_operand = (char*)calloc(256, sizeof(char));
    int iter_to_operand = 0;
    while (!isspace(*str) && is_operation(*str)!= 1 && *str!= '\0'){
        buffer_to_operand[iter_to_operand] = *str;
        iter_to_operand++;
        str++;
    }
    *iter = iter_to_operand;
    result = atof(buffer_to_operand);
    free(buffer_to_operand);
    return result;
}

double done_oper(double a, double b, char op){
    if(op == '+'){
        return a+b;
    } else if (op == '-'){
        return a-b;
    } else if (op == '*'){
        return a*b;
    } else if (op == '/'){
        return a/b;
    }
    return 0;
}

int is_unar_min(char *str){
    if(*str == '-' && is_operation(*(str+1)) == 0){
        return 1;
    } else
        return 0;
}

void resolve_unar_min(char *str, int *iter, D_Stack * stack_q){
    double res = 0 - create_double(str, iter);
    D_Stack_Push(stack_q, res);
}

int calcul(char *str, D_Stack *Q, Stack *W) {
    int iter = 0;
    double buf = 0;
    char op;
    double a=0, b=0;
    while (*str != '\0'){
        if (is_operation(*str) == -1){
            return -1;
        }
        if (isspace(*str)){
            if (is_unar_min(str+1)){
                resolve_unar_min((str+2), &iter, Q);
                str+=(iter+2);
            } else
                str++;
        } else if (is_operation(*str) == 0){
            buf = create_double(str, &iter);
            D_Stack_Push(Q, buf);
            str += iter;
        } else if (is_operation(*str) == 1){
            ///!!!!!!!!
            if (*str != ')' && (*(str+1) == '\0' || *(str+1) == '\n')){
                return -1;
            }
            if (W->real_size == 0){
                if (is_unar_min(str)){
                    resolve_unar_min(str+1, &iter, Q);
                    str+=(iter+1);
                }else{
                    Stack_Push(W, *str);
                    if (is_unar_min(str+1)){
                        resolve_unar_min((str+2), &iter, Q);
                        str+=(iter+2);
                    } else
                        str++;
                }
            } else {
                if (operation_prior(*str) == 1){
                    Stack_Push(W, *str);
                    if (is_unar_min(str+1)){
                        resolve_unar_min((str+2), &iter, Q);
                        str+=(iter+2);
                    } else
                        str++;
                } else if (operation_prior(*str) == 6){
                    while (1){
                        op = Stack_Pop(W);
                        if (op == '('){
                            break;
                        }
                        b = D_Stack_Pop(Q);
                        a = D_Stack_Pop(Q);
                        D_Stack_Push(Q, done_oper(a, b, op));
                    }
                    str++;
                }else if (operation_prior(W->symbol[W->real_size - 1]) >= operation_prior(*str)){
                    op = Stack_Pop(W);
                    b = D_Stack_Pop(Q);
                    a = D_Stack_Pop(Q);
                    D_Stack_Push(Q, done_oper(a, b, op));
                    Stack_Push(W, *str);
                    if (is_unar_min(str+1)){
                        resolve_unar_min((str+2), &iter, Q);
                        str+=(iter+2);
                    } else
                        str++;
                    //printf("not yet!\n");
                } else {
                    Stack_Push(W, *str);
                    if (is_unar_min(str+1)){
                        resolve_unar_min((str+2), &iter, Q);
                        str+=(iter+2);
                    } else
                        str++;
                }
                //temp/
                //str++;
            }

        } else {
            //printf("[error]");
            return 0;
        }
    }
    return 0;
}

double end_resolve(D_Stack *Q, Stack *W){

    if (Q->real_size <=0){
        printf("%s", "[error]");
        return -9999999999;
    }
    if (W->real_size == 0){
        return Q->symbol[Q->real_size-1];
    } else if (W->real_size < 0){
        printf("%s", "[error]");
        return -9999999999;
    } else {
        //double res = 0;
        while (W->real_size > 0) {
            double b = D_Stack_Pop(Q);
            double a = D_Stack_Pop(Q);
            char op = Stack_Pop(W);
            D_Stack_Push(Q, done_oper(a, b, op));
            //res=done_oper(a, b, op);
        }
        if (Q->real_size == 1){
            return D_Stack_Pop(Q);
        } else {
            printf("%s", "[error]");
            return -9999999999;
        }
    }
}

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
    fgets(str, MAX_STR, stdin);
    if(str == NULL){
        printf("%s", "[error]");
        return 0;
    }
    if ((str[strlen(str)-1]) == '\n'){
        str[strlen(str)-1] = '\0';
    }
    D_Stack *Q = NULL;
    Stack *W = NULL;
    Q = D_Stack_Init(Q, strlen(str));
    if (Q == NULL){
        printf("%s", "[error]");
        return 0;
    }
    W = Stack_Init(W, strlen(str));
    if (W == NULL){
        printf("%s", "[error]");
        return 0;
    }
    if(detect(str)){
        Stack_Free(W);
        D_Stack_Free(Q);
        printf("%s", "[error]");
        return 0;
    }
    int calc_res = calcul(str, Q, W);
    if (calc_res == -1) {
        Stack_Free(W);
        D_Stack_Free(Q);
        printf("%s", "[error]");
        return 0;
    }
    double result = end_resolve(Q, W);
    if (result == -9999999999){
        Stack_Free(W);
        D_Stack_Free(Q);
        return 0;
    }
    printf("%.2f", result);
    Stack_Free(W);
    D_Stack_Free(Q);
    return 0;
}