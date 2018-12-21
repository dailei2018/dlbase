#include "dl_base.h"
#include "t_base.h"

dl_pool *p;

void dump_str_arr(dl_array *arr){
    int i;
    dl_str *str;
    char *tmp;

    str = arr->elts;

    for(i = 0; i < arr->nelts; i++){

        tmp = dl_pnalloc(p, str->len+1);
        memcpy(tmp, str->data, str->len);
        tmp[str->len] = '\0';

        dl_printf("[%s]\n", tmp);
        str++;
    }

}

int main(){
    dl_array *arr;

    p = dl_create_pool(1024, NULL);
    if(p == NULL) return 1;

    arr = dl_array_create(p, 8, sizeof(dl_str));
    if(arr == NULL) return 1;

    dl_str str = dl_string("abcd,skdfjskdlfj, , ,,123,4");

    if(dl_split_by_c(arr, &str, ',') == NULL) return 1;


    dump_str_arr(arr);


    dl_destroy_pool(p);
    return 0;
}
