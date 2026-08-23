#include <string.h>
#include "script.h"


int make_vectors(char* filebuf, struct macro_vector mv[]){
    void* init_v = 0;
    void* frame_v = 0;
    register int i = 0, mvcnt = 0;

    while(*filebuf){
        if(*filebuf == '\n'){
            filebuf++;
            if(*filebuf == '{'){
                filebuf++;
                if(!(strncmp(filebuf, "init}", 5))){
                    filebuf += 5;
                    if(init_v) error(10);
                    init_v = filebuf;
                }
                else if(!(strncmp(filebuf, "frame}", 6))){
                    filebuf += 6;
                    if(frame_v) error(11);
                    frame_v = filebuf;
                }
                else if(!(strncmp(filebuf, "macro:", 6))){
                    mv[mvcnt].id1 = mv[mvcnt].id2 = 0;
                    filebuf += 6;
                    i = 0;
                    while(*filebuf != '}'){
                        mv[mvcnt].id1 += *filebuf;
                        if(i % 2) mv[mvcnt].id2 += *filebuf;
                        filebuf++;
                        i++;
                    }
                    mv[mvcnt].vector = filebuf;
                    mvcnt++;
                }
            }
        }
        filebuf++;
    }
    return mvcnt;
}