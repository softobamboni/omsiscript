extern void error(char code);

struct macro_vector{
    unsigned id1;
    unsigned id2;
    char* vector;
};
struct var_ids{
    unsigned id1;
    unsigned id2;
};
struct const_def{
    unsigned id1;
    unsigned id2;
    float value;
};
struct coords{
    float x;
    float y;
};
struct line_func{
    unsigned id1;
    unsigned id2;
    struct coords pnt[64];
};

extern int make_vectors(char* filebuf, struct macro_vector mv[]);
extern int id_vars(const char* varlist_fname, char type, struct var_ids varid[], void* vars[]);
extern void init_const(const char* filename, struct const_def const_ptr[], struct line_func lf[], struct const_def **newptr, struct line_func **nptr_func, int *ccptr, int *fcptr);