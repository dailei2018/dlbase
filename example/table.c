#include <stdio.h>
#include <assert.h>

#include "dl_base.h"

dl_log      *logger;
dl_pool     *pool;
dl_table    *tb;

void dl_void_del_h(void *d)
{
    printf("dl_void_del_h called\n");
    free(d);
}

void dl_print_value(dl_value_l *v)
{
    if (v == NULL);
}

int main()
{
    int rc;
    
    logger = dl_log_init(DL_LOG_DEBUG, NULL);
    if (logger == NULL) return -1;
    
    
    tb = dl_table_new(logger, 0, 0, dl_void_del_h);
    if (tb == NULL) return -1;
    
    //tb['name'] = 'dl'
    rc = dl_node_set_str_l(tb, "name", sizeof("name") - 1, "dl", sizeof("dl") - 1);
    if (rc == -1) return -1;
    
    //tb['age'] = 27
    rc = dl_node_set_int_l(tb, "age", sizeof("age") - 1, 27);
    if (rc == -1) return -1;
    
    // tb[0] = 'hello'
    rc = dl_array_set_str_l(tb, 0, "hello", sizeof("hello") - 1);
    if (rc == -1) return -1;
    
    // tb[2] = 'hello'
    rc = dl_array_set_str_l(tb, 2, "hello", sizeof("hello") - 1);
    if (rc == -1) return -1;
    
    // tb[2] = 'hello_replace'
    rc = dl_array_set_str_l(tb, 2, "hello_replace", sizeof("hello_replace") - 1);
    if (rc == -1) return -1;
    
    char *title = strdup("table test");
    //tb['void'] = title
    rc = dl_node_set_void_l(tb, "void", sizeof("void") - 1, title);
    if (rc == -1) return -1;
    
    
    
    // fine test
    
    dl_value_l  *v;
    
    v = dl_find_by_index(tb, 0);
    if (v == NULL)
        printf("tb[0] == NULL\n");
    else {
        assert(v->tt == DL_STR_L);
        dl_printf("tb[0] == '%V'\n", v->v.s);
    }
    
    v = dl_find_by_index(tb, 1);
    if (v == NULL)
        printf("tb[1] == NULL\n");
    
    v = dl_find_by_index(tb, 2);
    if (v == NULL)
        printf("tb[2] == NULL\n");
    else {
        assert(v->tt == DL_STR_L);
        dl_printf("tb[2] == '%V'\n", v->v.s);
    }
    
    v = dl_find_by_str(tb, "name", sizeof("name") - 1);
    if (v == NULL)
        printf("tb['name'] == NULL\n");
    else {
        assert(v->tt == DL_STR_L);
        dl_printf("tb['name'] == '%V'\n", v->v.s);
    }
    
    // replace tb['void'] to 1111, will call dl_void_del_h
    rc = dl_node_set_int_l(tb, "void", sizeof("void") - 1, 1111);
    if (rc == -1) return -1;
    
    printf("\n\n-------dump\n");
    dl_dump_talbe_l(tb);
    
    return 0;
}