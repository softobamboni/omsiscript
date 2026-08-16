#include <complex.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>

float stack[8], reg[8];
char stackptr = 7;
char str0[256], str1[256], str2[256], str3[256], str4[256], str5[256], str6[256], str7[256];
char* strstack[8] = {str0,str1,str2,str3,str4,str5,str6,str7};
char strptr = 7;

char push(float val){ // push value into stack
    if(stackptr < 0) return 1;
    if(stackptr > 7) return 2;
    stack[stackptr--] = val;
    printf("\n%f\n", stack[stackptr+1]);
    return 0;
}
float pop(){ // pop value out of stack
    printf("\n%f\n", stack[stackptr+1]); 
    return stack[++stackptr];
}

uint strpop(char* dest){
    if(strptr >= 7) return 0; //stack underflow
    uint i = 0;
    strptr++;
    while((dest[i] = (strstack[strptr])[i])) i++;
    return i;
}

float sign(float val){
    if(val == 0.f) return val;
    if(val > 0.f) return 1.;
    return -1.;
}

char* strcut(char* str, uint n){
    if(n > 256) return 0;
    uint i = 0;
    do{
      str[i] = str[n+i];
      i++;
    }
    while(str[n+i]);
    return str;
}
char* strtrunc(char* str, int n){
    if(n > 256) return 0;
    str[strlen(str) - n] = 0;
    return str;
}

int main(){
    float t0, t1;

    FILE* osc = fopen("test.osc", "r");
    char linebuf[256], wordbuf[256], saux0[256], saux1[256];
    char* charp; char* auxp;
    uint i = 0, j;

    srand(time(NULL));
	
	while(fgets(linebuf, 255, osc)){
		charp = linebuf; // reset moving pointer to start
		while(*charp){
			switch(putchar(*charp)){
				case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': case '0': //stores number to stack
                    if(push(strtof(charp, &charp))) exit(1);
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
                    if(*(charp+1) != '-'){
                        t0 = pop();
                        t1 = pop();
                        push( ( t1 / t0 ) );
                    }
                    else{ // its /-/ probably
                        push(-(pop()));
                        charp += 2;
                    }
                    break;
                case '"': //push string to stack
                    i = 0;
                    while(*(++charp) != '"'){
                      *(i + strstack[strptr]) = *charp;
                      i++;
                    }
                    *(strstack[strptr]+i) = 0;
                    printf("\n%s\n", strstack[strptr--]);
                    break;
                case '(': //macros, variables...
                case '{': //if condition
                case '$': //string operations
                    charp++;
                    switch (putchar(*charp)){
                        case '+':
                            i = 0;
                            strpop(saux0);
                            strpop(saux1);
                            printf("\n%s\n",strcat(saux1,saux0));
                            while(((strstack[strptr])[i] = saux1[i])) i++;
                            strptr--;
                            break;
                        case '*':
                            i = strpop(saux0);// str length
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
                            break;
                        default:
                            i = 0;
                            while((*charp != ' ') && *charp){
                                wordbuf[i] = *charp++;
                                i++;
                            }
                            wordbuf[i] = 0;
                            //all suitable operations down there:
                            if(!(strcmp(wordbuf,"length"))) push(strlen(strstack[strptr]));
                            else if(!(strcmp(wordbuf,"cutBegin"))) strcut(strstack[strptr], pop());
                            else if(!(strcmp(wordbuf,"cutEnd"))) strtrunc(strstack[strptr],pop());
                            else if(!(strcmp(wordbuf,"SetLengthR"))) push(sqrtf(pop()));
                            else if(!(strcmp(wordbuf,"SetLengthC"))){
                                t0 = pop();
                                t1 = pop();
                                push(t0 < t1?t0:t1);
                            }
                            else if(!(strcmp(wordbuf,"SetLengthL"))){
                                t0 = pop();
                                t1 = pop();
                                push(t0 < t1?t1:t0);
                            }
                            else if(!(strcmp(wordbuf,"IntToStr")))push(expf(pop()));
                            else if(!(strcmp(wordbuf,"IntToStrEnh"))){ t0 = pop(); push(t0*t0); }
                            else if(!(strcmp(wordbuf,"StrToFloat")))push(strtof(strstack[strptr], 0));
                            else if(!(strcmp(wordbuf,"RemoveSpaces")))push(M_PI);
                    }
                case '\n': case ' ': break;
                default: //handle operations with latin names (sin, trunc...)
                    i = 0;
                    while((*charp != ' ') && *charp){
                        wordbuf[i] = *charp++;
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
                    else if(!(strcmp(wordbuf,"pi")))push(M_PI);
                    else if(!(strcmp(wordbuf,"random")))push(rand() % (uint)pop());
                    else if(!(strcmp(wordbuf,"abs")))push(fabs(pop()));
                    else if(!(strcmp(wordbuf,"trunc")))push((int)pop());

            }
            if(!*charp)charp--;
            charp++;
            for(i = 0; i < 8; i++){
                printf("%f ", stack[i]);
            }
            putchar('\n');
        }
        printf("\n|%s", strstack[strptr+1]);
    }
    exit(0);
}

