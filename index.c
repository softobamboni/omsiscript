#include <string.h>
#include <stdlib.h>
#include "script.h"


int make_vectors(char* filebuf, char** init_v, char** frame_v, struct macro_vector *mvptr[], struct macro_vector *tvptr[]){
//    void* init_v = 0;
//    void* frame_v = 0;
    register int i = 0, mvcnt = 0, tvcnt = 0;

    while(*filebuf){
        if(*filebuf == '\n'){
            filebuf++;
            if(*filebuf == '{'){
                filebuf++;
                if(!(strncmp(filebuf, "init}", 5))){
                    filebuf += 5;
                    if(*init_v) error(10);
                    *init_v = filebuf;
                }
                else if(!(strncmp(filebuf, "frame}", 6))){
                    filebuf += 6;
                    if(*frame_v) error(11);
                    *frame_v = filebuf;
                }
                else if(!(strncmp(filebuf, "macro:", 6))){
                    *mvptr = realloc(*mvptr, (2*sizeof(unsigned) + sizeof(char*)) * (mvcnt+1));
                    (*mvptr)[mvcnt].id1 = (*mvptr)[mvcnt].id2 = 0;
                    filebuf += 6;
                    i = 0;
                    while(*filebuf != '}'){
                        (*mvptr)[mvcnt].id1 += *filebuf;
                        if(i % 2) (*mvptr)[mvcnt].id2 += *filebuf;
                        filebuf++;
                        i++;
                    }
                    (*mvptr)[mvcnt].vector = filebuf+1;
                    mvcnt++;
                }
                else if(!(strncmp(filebuf, "trigger:", 8))){
                    *tvptr = realloc(*tvptr, (2*sizeof(unsigned) + sizeof(char*)) * (tvcnt+1));
                    (*tvptr)[tvcnt].id1 = (*tvptr)[tvcnt].id2 = 0;
                    filebuf += 8;
                    i = 0;
                    while(*filebuf != '}'){
                        (*tvptr)[tvcnt].id1 += *filebuf;
                        if(i % 2) (*tvptr)[tvcnt].id2 += *filebuf;
                        filebuf++;
                        i++;
                    }
                    (*tvptr)[tvcnt].vector = filebuf+1;
                    tvcnt++;
                }
            }
        }
        filebuf++;
    }


    return mvcnt;
}
