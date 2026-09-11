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
    struct coords *pnt;
};

struct parser_args{
    char* filename_osc;
    char* filename_constfile;
    char* filename_varlist;
    char* filename_stringvarlist;
    struct var_ids *global_vid;
    float* global_vars;
    int* global_varcnt;
};

struct exec_args{
    struct macro_vector *mv;
    struct macro_vector *tv;
    struct var_ids *vid;
    struct const_def *const_ptr;
    struct line_func *func_ptr;
    float* vars;
    struct var_ids *global_vid;
    float* global_vars;
    int mvcnt;
    int varcnt;
    int constcnt;
    int funcnt;
    int* global_varcnt;
};

void* parse_func(void* args);
int make_vectors(char* filebuf, char** init_v, char** frame_v, struct macro_vector *mvptr[], struct macro_vector *tvptr[]);
int id_vars(const char* varlist_fname, char type, struct var_ids *varid[], void* vars[], unsigned long* size_t, unsigned long* size_v);
void init_const(const char* filename, struct const_def *const_ptr[], struct line_func *lf[], int *ccptr, int *fcptr);
void error(char code);
void getfile(const char* fname, char** filebuf, unsigned long* prev_size);
