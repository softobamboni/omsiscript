#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "script.h"

int id_vars(const char* varlist_fname, char type, struct var_ids *varid[], void* vars[], unsigned long* size_t, unsigned long* size_v){ //type: 0 = float, 1 = string
    FILE* varlist = fopen(varlist_fname,"r");
    if(!varlist) error(20);

    char buf[1024]; 
    register int i, varcnt =, size, oldsize_t;
    if(size_t) oldsize_t = *size_t;
    else oldsize_t = 0;

    if(size_v) varcnt = *size_v;
    else varcnt = 0;

    size = type?1024:sizeof(float);

    while(fgets(buf, 1024, varlist)){
        if((isspace(buf[0]))) continue;
        *varid = realloc(*varid, oldsize_t + (sizeof(unsigned) * 2 * (varcnt+1)));
        i = (*varid)[varcnt].id1 = (*varid)[varcnt].id2 = 0;
        while(!(isspace(buf[i]))){
            (*varid)[varcnt].id1 += buf[i];
            if(i % 2) (*varid)[varcnt].id2 += buf[i];
            i++;
        }
//        printf("\nChecksum: %u; \nOdd letter checksum: %u;\n", varid[varcnt].id1, varid[varcnt].id2);
        varcnt++;
    }
    *vars = realloc(*vars, varcnt*size);
    *size_t = oldsize_t + (sizeof(unsigned) * 2 * (varcnt+1));
    if(*vars) printf("\nAllocated %d bytes for variables", varcnt*size);
    return varcnt;
}
