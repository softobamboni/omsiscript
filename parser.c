
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "script.h"

#define PI 3.14159265
float stack[8], reg[8];
char stackptr = 7;
char str0[256], str1[256], str2[256], str3[256], str4[256], str5[256], str6[256], str7[256];
char* strstack[8] = {str0,str1,str2,str3,str4,str5,str6,str7};
char strptr = 7;
struct macro_vector mv[256];
struct var_ids vid[1024];
float* vars = 0;
int mvcnt, varcnt;

void error(char code){
    putchar('\n');
    switch(code){//various errors or exceptions
        case 1: printf("Stack overflow!"); break;
        case 2: printf("Stack underflow!"); break;
        case 3: printf("String stack underflow!"); break;
        case 4: printf("String length out of range!"); break;
        default: printf("Unknown error!");
    }
    exit(code);
}

static inline void push(float val){ // push value into stack
    if(stackptr < 0) error(1);
    if(stackptr > 7) error(2);
    stack[stackptr--] = val;
    printf("\n%f\n", stack[stackptr+1]);
}
static inline float pop(){ // pop value out of stack
    printf("\n%f\n", stack[stackptr+1]); 
    return stack[++stackptr];
}

static inline char* strpop(char* dest){
    if(strptr >= 7) error(3); //stack underflow
    int i = 0;
    strptr++;
    while((dest[i] = (strstack[strptr])[i])) i++;
    return dest;
}

static inline float sign(float val){
    if(val == 0.f) return val;
    if(val > 0.f) return 1.;
    return -1.;
}

static inline char* strcutl(char* str, int n){
    if(n > 256) return 0;
    int i = 0;
    do{
      str[i] = str[n+i];
      i++;
    }
    while(str[n+i]);
    str[i] = 0;
    return str;
}
static inline void strcutr(char* str, int n){
    str[strlen(str) - n] = 0;
}

static inline void stresizeL(char* str, int n){
    int len = strlen(str);
    if(len > n) str[n] = 0;
    else if(len < n){
        do str[len++] = ' '; while(len < n);
    }
}


static inline void stresizeR(char* str, int n){
    int len = strlen(str);
    if(len > n) strcutl(str,n);
    else if(len < n){
        do{
            str[n--] = str[len--]; // move string to the right
        }
        while(len);
        do{
            str[n--] = ' '; // fill in gap in the left with spaces
        }
        while(n);
    }
}

static inline void inttostr(int n, char* str, const char* fmt){
    int dcount = 0, target_dcount = fmt?strtof(fmt+1, 0):0, i = 0;
    char buf[256];

    if(n < 0) buf[dcount++] = '-';
    do{
        buf[dcount++] = (n % 10) + '0';
        n /= 10;
    }
    while(n);
    target_dcount -= dcount;
    if(target_dcount > 0) while(target_dcount--) str[i++] = fmt[0];
    if(buf[0] == '-') str[i++] = '-';
    while(dcount) str[i++] = buf[--dcount];
    str[i] = 0;
}

static inline void strcls(char* str){
    int i = strlen(str);
    while(str[--i] == ' ');
    str[i+1] = 0;
    i = 0;
    while(str[i] == ' ') i++;
    if(!strcutl(str,i)) exit(1);
}

void getfile(const char* fname, char** filebuf){
    FILE* osc = fopen(fname, "rb");
    if(!osc) exit(6);
    fseek(osc,0,SEEK_END);
    unsigned long int fsize = ftell(osc) + 1;

    *filebuf = malloc(fsize);

    rewind(osc);
    if(!fread(*filebuf, 1, fsize, osc)) error(7);
    fclose(osc);
}

char* exec(char* filebuf, char arg){ // arg: 0 = regular code, 1 = if condition, 2 = else condition
    register float t0, t1;

    char wordbuf[256], saux0[256], saux1[256];
    char* auxp; register char iflevel = 0, elselevel = 0;
    register int i = 0, j = 0;

    while(*filebuf){
        switch(*filebuf){
            case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': case '0': //stores number to stack
                push(strtof(filebuf, &filebuf));
                break;
            case '+':
                push( ( pop() + pop() ) );
                break;
            case '-':
                t0 = pop();
                t1 = pop();
                push( ( t1 - t0 ) );
                break;
            case '*':
                push((pop()*pop()));
                break;
            case '/':
                if(*(filebuf+1) != '-'){
                    t0 = pop();
                    t1 = pop();
                    push( ( t1 / t0 ) );
                }
                else{ // its /-/ probably
                    push(-(pop()));
                    filebuf += 2;
                }
                break;
            case '&':
                filebuf++;
                push( pop() && pop() );
                break;
            case '|':
                filebuf++;
                push( pop() || pop() );
                break;
            case '!':
                push( !pop() );
                break;
            case '=': //are 2 stack values equal?
                push( pop() == pop() );
                break;
            case '<': //is s1 less than s0?
                t0 = pop();
                t1 = pop();
                i = t1 < t0;
                if(*(filebuf+1) == '=') {i += t0 == t1; filebuf++;}
                push(i);
                break;
            case '>': //is s1 less than s0?
                t0 = pop();
                t1 = pop();
                i = t1 > t0;
                if(*(filebuf+1) == '=') {i += t0 == t1; filebuf++;}
                push(i);
                break;
            case '"': //push string to stack
                i = 0;
                while(*(++filebuf) != '"'){
                    *(i + strstack[strptr]) = *filebuf;
                    i++;
                }
                *(strstack[strptr]+i) = 0;
                printf("\n%s\n", strstack[strptr--]);
                break;
            case '(': //macros, variables...
                filebuf++;
                unsigned id1 = 0, id2 = 0;
                switch(*filebuf){
                    case 'M':
                        filebuf += 4; //temporarily to skip ".L."
                        i = 0;
                        while(*filebuf != ')'){
                            id1 += *filebuf; // calculate teh checksum of teh macro name
                            if(i % 2) id2 += *filebuf;
                            filebuf++;
                            i++;
                        }
                        i = 0;
                        while(i < mvcnt){
                            if(id1 == mv[i].id1) if(id2 == mv[i].id2) {printf("\nFound macro #%d", i); exec(mv[i].vector, 0);} // search for teh checksum
                            i++;
                        }
                        break;
                    case 'L': // load variable
                        j = 1; //j is used as load flag
                    case 'S': // store variable
                        filebuf += 2;
                        if(*filebuf == 'L'){ //local var
                            filebuf += 2;
                            i = 0;
                            while(*filebuf != ')'){
                                id1 += *filebuf; // calculate teh checksum of teh var name
                                if(i % 2) id2 += *filebuf;
                                filebuf++;
                                i++;
                            }
                            i = 0;
                            while(i < varcnt){
                                if(id1 == vid[i].id1) if(id2 == vid[i].id2) {
                                    printf("\nVariable num: %d", i); 
                                    if(j) push(vars[i]);
                                    else vars[i] = stack[stackptr+1];
                                }
                                i++;
                            }
                        }
                        j = 0;
                }
                break;
            case '{': //if conditions and return operations
                if(!strncmp(filebuf+1, "if}", 3)){ // this code is a bit goofy
                    filebuf += 4;
                    if(stack[stackptr+1]) filebuf = exec(filebuf, 1);
                    else{ //glide to next else condition and increase nest level to prevent jumping to else condition that is inside this if condition
                        iflevel++;
                        while(*(filebuf) != '{') filebuf++;
                        filebuf--;
                    }
                }
                if(!strncmp(filebuf+1, "else}", 5)){
                    filebuf += 6;
                    if(iflevel == 1) filebuf = exec(filebuf, 2);
                    if(arg == 1){ //glide to next else condition and increase nest level to prevent jumping to else condition that is inside this if condition
                        elselevel++;
                        while((*filebuf != '{') || (*(filebuf+1) != 'e')) filebuf++;
                        filebuf--;
                    }
                }
                else if(!strncmp(filebuf+1, "endif}",6 )){
                    filebuf += 7;
                    if(arg && !iflevel && !(elselevel-1)) return filebuf;
                    if(iflevel){
                        iflevel--;
                        while(*(filebuf) != '{') filebuf++;
                        filebuf--;
                    }
                    if(elselevel){
                        elselevel--;
                        while(*(filebuf) != '{') filebuf++;
                        filebuf--;
                    }
                }
                else if(!strncmp(filebuf+1, "end}",4 )){
                    return filebuf+5;
                }
                break;
            case '$': //string operations
                filebuf++;
                switch (*filebuf){
                    case '+':
                        i = 0;
                        strpop(saux0);
                        strpop(saux1);
                        printf("\n%s\n",strcat(saux1,saux0));
                        while(((strstack[strptr])[i] = saux1[i])) i++;
                        strptr--;
                        break;
                    case '*':
                        i = strlen(saux0);// str length
                        j = pop() / i;
                        printf("\n%d\n",j);
                        if(j <= 1) break;
                        auxp = strstack[strptr] + i;
                        while(--j){
                            i = 0;
                            while((*auxp++ = saux0[i++]));
                            auxp--;
                        }
                        strptr--;
                        j = 0;
                        break;
                    case '=': //are 2 stack values equal?
                        if(!strcmp(strpop(saux0),strpop(saux1))) push(1);
                        else push(0);
                        break;
                    case '>': //is s1 less than s0?
                        i = strcmp(strpop(saux0),strpop(saux1));
                        if(*(filebuf+1) == '=') {filebuf++; i -= 1;} // 0 becomes -1
                        push(i < 0?1:0);
                        break;
                    case '<': //is s1 less than s0?
                        i = strcmp(strpop(saux0),strpop(saux1));
                        if(*(filebuf+1) == '=') {filebuf++; i += 1;} // 0 becomes -1
                        push(i > 0?1:0);
                        break;
                    case 'd': if(isspace(*(filebuf+1))){
                        strcpy(strstack[strptr], strstack[strptr+1]);
                        strptr--;
                        break;
                    }
                    default:
                        i = 0;
                        while((*filebuf != ' ') && (*filebuf != '\n') && *filebuf){
                            wordbuf[i] = *filebuf++;
                            i++;
                        }
                        wordbuf[i] = 0;
                        //all suitable operations down there:
                        if(!(strcmp(wordbuf,"length"))) push(strlen(strstack[strptr+1]));
                        else if(!(strcmp(wordbuf,"cutBegin"))) strcutl(strstack[strptr+1], pop());
                        else if(!(strcmp(wordbuf,"cutEnd"))) strcutr(strstack[strptr+1],pop());
                        else if(!(strcmp(wordbuf,"SetLengthR"))) stresizeR(strstack[strptr+1],stack[stackptr+1]);
                        else if(!(strcmp(wordbuf,"SetLengthC"))){
                            i = stack[stackptr+1] / 2;
                            stresizeL(strstack[strptr+1],i);
                            stresizeR(strstack[strptr+1],stack[stackptr+1]-i);
                        }
                        else if(!(strcmp(wordbuf,"SetLengthL"))) stresizeL(strstack[strptr+1],stack[stackptr+1]);
                        else if(!(strcmp(wordbuf,"IntToStr"))) inttostr(pop(), strstack[strptr--], 0);
                        else if(!(strcmp(wordbuf,"IntToStrEnh"))) {inttostr(pop(), strstack[strptr], strstack[strptr+1]); strptr--;}
                        else if(!(strcmp(wordbuf,"StrToFloat"))) push(strtof(strstack[++strptr],0));
                        else if(!(strcmp(wordbuf,"RemoveSpaces"))) {puts(strstack[strptr+1]); strcls(strstack[strptr+1]);}
                    }
            case '\'': while(*filebuf != '\n') filebuf++;
            case '\n': case ' ': case '\t': case '}': break;
            case 'd':
            if(isspace(*(filebuf+1))){
                push(stack[stackptr+1]);
            }
            break;
            case 's':
            if(isdigit(*(filebuf+1))){//save (write) to register
                filebuf++;
                i = *filebuf - '0';
                reg[i] = stack[stackptr+1];
            }
            break;
            case 'l':
            if(isdigit(*(filebuf+1))){//load (read) from register
                filebuf++;
                i = *filebuf - '0';
                push(reg[i]);
            }
            break;
            default: //handle operations with latin names (sin, trunc...)
                i = 0;
                while((*filebuf != ' ') && (*filebuf != '\n') && *filebuf){
                    wordbuf[i] = *filebuf++;
                    i++;
                }
                wordbuf[i] = 0;
                //all suitable operations down there:
                if(!(strcmp(wordbuf,"sin"))) push(sinf(pop()));
                else if(!(strcmp(wordbuf,"arcsin"))) push(asinf(pop()));
                else if(!(strcmp(wordbuf,"arctan"))) push(atanf(pop()));
                else if(!(strcmp(wordbuf,"sqrt"))) push(sqrtf(pop()));
                else if(!(strcmp(wordbuf,"min"))){
                    t0 = pop();
                    t1 = pop();
                    push(t0 < t1?t0:t1);
                }
                else if(!(strcmp(wordbuf,"max"))){
                    t0 = pop();
                    t1 = pop();
                    push(t0 < t1?t1:t0);
                }
                else if(!(strcmp(wordbuf,"exp")))push(expf(pop()));
                else if(!(strcmp(wordbuf,"sqr"))){ t0 = pop(); push(t0*t0); }
                else if(!(strcmp(wordbuf,"sgn")))push(sign(pop()));
                else if(!(strcmp(wordbuf,"pi")))push(PI);
                else if(!(strcmp(wordbuf,"random"))){
                    i = rand() % (unsigned)pop();
                    push(i);

                }
                else if(!(strcmp(wordbuf,"abs")))push(fabs(pop()));
                else if(!(strcmp(wordbuf,"trunc")))push((int)pop());

            }
        if(!*filebuf)return filebuf;
        filebuf++;
    }
    return filebuf;
}


int main(){
    char* filebuf;
    getfile("MultiGbxScript.osc",&filebuf);
    srand(time(NULL));
//    exec(filebuf, 0);
    mvcnt = make_vectors(filebuf, mv);
    varcnt = id_vars("MultiGbxScript_varlist.txt", 0, vid, (void**)&vars);
    exec(filebuf, 0);
    return 0;
}
