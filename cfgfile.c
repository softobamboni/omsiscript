#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "script.h"

void parse_cfg(const char* filename, struct const_def *const_ptr[], struct line_func *lf[], int *ccptr, int *fcptr){
    FILE* f = fopen(filename,"r");
    char* filebuf = 0;

    if(!f) error(30);

    char buf[1024]; 
    register unsigned long i = 0, j=0;
    unsigned long cur_size = 0;
    struct coords xy_buf[256];

    while(fgets(buf,1024,f)){
        if(*buf == '['){
            if(!strncmp(buf+1,"script]",7)){
                fgets(buf,1024,f);

                i = strtoul(buf,NULL,10);

                while(i--){
                    fgets(buf,1024,f);
                    while(!isspace(buf[j++]));
                    buf[j-1] = 0;
                    getfile(buf, &filebuf, &cur_size); // combining i script files into one big one
                }
            }
            else if(!strncmp(buf+1,"varnamelist]",12)){
                fgets(buf,1024,f);

                i = strtoul(buf,NULL,10);

                while(i--){

                }
            }
            else if(!strncmp(buf+1,"pnt]",4)){
                fgets(buf,1024,f);
                xy_buf[pntcnt].x = strtof(buf,NULL);

                fgets(buf,1024,f);
                xy_buf[pntcnt].y = strtof(buf,NULL);

                pntcnt++;
            }
        }
    }
    *ccptr = constcnt;
    *fcptr = funcnt;
    printf("\nAllocated %zu + %zu bytes for constables", sizeof(*(*const_ptr)) * constcnt, cur_size + pntcnt * 2*sizeof(float));
}
