#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "script.h"

int id_vars(const char* varlist_fname, char type, struct var_ids varid[], void* vars[]){ //type: 0 = float, 1 = string
    FILE* varlist = fopen(varlist_fname,"r");
    if(!varlist) error(20);

    char buf[1024]; 
    register int i, varcnt = 0, size;
    size = type?256:sizeof(float);

    while(fgets(buf, 1024, varlist)){
        if((isspace(buf[0]))) continue;
        i = varid[varcnt].id1 = varid[varcnt].id2 = 0;
        while(!(isspace(buf[i]))){
            varid[varcnt].id1 += buf[i];
            if(i % 2) varid[varcnt].id2 += buf[i];
            i++;
        }
//        printf("\nChecksum: %u; \nOdd letter checksum: %u;\n", varid[varcnt].id1, varid[varcnt].id2);
        varcnt++;
    }
    *vars = calloc(varcnt, size);
    if(*vars) printf("\nAllocated %zd bytes for variables", varcnt*sizeof(float));
    return varcnt;
}