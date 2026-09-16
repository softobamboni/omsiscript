#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "script.h"

void init_const(const char* filename, struct const_def *const_ptr[], struct line_func *lf[], int *ccptr, int *fcptr, unsigned* cur_constcnt, unsigned long* old_size){
    FILE* constfile = fopen(filename,"r");
    if(!constfile) error(20);

    char buf[1024]; 
    register unsigned i = 0, constcnt, funcnt = 0, pntcnt = 0, id1 = 0, id2 = 0;
    register unsigned long cur_size;
    struct coords xy_buf[256];

    if(cur_constcnt) constcnt = *cur_constcnt;
    else constcnt = 0;

    if(old_size) cur_size = *old_size;
    else cur_size = 0;

    while(fgets(buf,1024,constfile)){
        if(*buf == '['){
            if(!strncmp(buf+1,"const]",6)){
                fgets(buf,1024,constfile);

                constcnt++;
                *const_ptr = realloc(*const_ptr, constcnt * sizeof(struct const_def));

                id1 = id2 = i = 0;
                while(!isspace(buf[i])){
                    id1 += buf[i];
                    if(i % 2) id2 += buf[i];
                    i++;
                }
                (*const_ptr)[constcnt-1].id1 = id1;
                (*const_ptr)[constcnt-1].id2 = id2;

                fgets(buf,1024,constfile);
                (*const_ptr)[constcnt-1].value = strtof(buf, NULL);
            }
            else if(!strncmp(buf+1,"newcurve]",9)){
                fgets(buf,1024,constfile);

                if(pntcnt){ //store pending function
                    cur_size += 3*sizeof(struct line_func);
                    *lf = realloc(*lf, cur_size);
                    (*lf)[funcnt-1].id1 = id1;
                    (*lf)[funcnt-1].id2 = id2;

                    (*lf)[funcnt-1].pnt = malloc(pntcnt * 2*sizeof(float));
                    for(i = 0; i < pntcnt; i++) {
                        (*((*lf)[funcnt-1].pnt+i)).x = xy_buf[i].x; // didn't figure out how 2 get the Flexible Array Member™ to work
                        (*((*lf)[funcnt-1].pnt+i)).y = xy_buf[i].y; // so i malloc'd some space for an array for function points and accessed it with that mess of parentheses and dereference operators
                    }
                    pntcnt = 0;
                }

                funcnt++;
                id1 = id2 = i = 0;
                while(!isspace(buf[i])){
                    id1 += buf[i];
                    if(i % 2) id2 += buf[i];
                    i++;
                }
            }
            else if(!strncmp(buf+1,"pnt]",4)){
                fgets(buf,1024,constfile);
                xy_buf[pntcnt].x = strtof(buf,NULL);

                fgets(buf,1024,constfile);
                xy_buf[pntcnt].y = strtof(buf,NULL);

                pntcnt++;
            }
        }
    }
    *ccptr = constcnt;
    *fcptr = funcnt;
    printf("\nAllocated %zu + %zu bytes for constables", sizeof(*(*const_ptr)) * constcnt, cur_size + pntcnt * 2*sizeof(float));
}
