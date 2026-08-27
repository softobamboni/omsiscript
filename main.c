#include <pthread.h>
#include "script.h"

int main(){
    pthread_t test, test2;

    struct var_ids global_vid;
    float global_vars[256];
    int global_varcnt = 0;

    struct parser_args args;
    args.filename_constfile = "/mnt/warehouse/omsiscript/MultiGbxScript_consfile.txt";
    args.filename_varlist = "MultiGbxScript_varlist.txt";
    args.filename_osc = "MultiGbxScript.osc";
    args.filename_stringvarlist = NULL;
    args.global_vars = global_vars;
    args.global_vid = &global_vid;
    args.global_varcnt = &global_varcnt;

    struct parser_args args2;
    args2.filename_constfile = "Vmatrix_constfile.txt";
    args2.filename_varlist = "Vmatrix_varlist.txt";
    args2.filename_osc = "VMatrixP.osc";
    args2.filename_stringvarlist = NULL;
    args2.global_vars = global_vars;
    args2.global_vid = &global_vid;
    args2.global_varcnt = &global_varcnt;

    int retval = pthread_create(&test, NULL, parse_func, &args);
    int retval2 = pthread_create(&test2, NULL, parse_func, &args2);
//    parse_func(&args2);
    pthread_join(test, NULL);
    pthread_join(test2, NULL);
    return retval;
}
