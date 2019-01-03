#include "dl_base.h"

pcre2_code *
dl_compile_re(char *pattern, size_t len, dl_log *log){
    PCRE2_SIZE  erroroffset;
    int errornumber;
    pcre2_code *re;
    
    re = pcre2_compile(pattern, len, 0, &errornumber, &erroroffset, NULL);
    if(re == NULL){
        if(log){
            PCRE2_UCHAR buffer[256];
            pcre2_get_error_message(errornumber, buffer, sizeof(buffer));
            dl_log_error(DL_LOG_ERR, log,
                        "PCRE2 compilation failed at offset %d: %s", (int)erroroffset, buffer);
        }
            
        return NULL;
    }
    
    return re;
}

/* replace string */
dl_str *
dl_str_replace_re(dl_pool *p, char *s1, size_t len,
                   char *pattern,
                   char *s3, size_t len_3, pcre2_code *rep)
{
    dl_pool     *tmp_p;
    dl_str      *str, *res_s;
    char        *res, *cur;
    dl_list     *list;
    dl_list_part    *part;
    
    PCRE2_SIZE  erroroffset;
    PCRE2_SIZE  *ovector;
    int total, errornumber, rc, offset, option;
    pcre2_match_data    *match_data;
    pcre2_code          *re;
    
    re = rep;
    
    res_s = dl_palloc(p, sizeof(dl_str));
    if(*pattern == '\0'){
        res_s->data = dl_pstrdup(p, s1, len);
        res_s->len = len;
        
        return res_s;
    }
    
    tmp_p = dl_create_pool(1024, NULL);
    if(tmp_p == NULL) return NULL;
    
    list = dl_list_create(tmp_p, 1, sizeof(dl_str));
    if(list == NULL) return NULL;
    
    if(rep == NULL){
        re = pcre2_compile(pattern, PCRE2_ZERO_TERMINATED, 0, &errornumber, &erroroffset, NULL);
        if(re == NULL){
            PCRE2_UCHAR buffer[256];
            pcre2_get_error_message(errornumber, buffer, sizeof(buffer));
            dl_log_error(DL_LOG_ERR, p->log,
                        "PCRE2 compilation failed at offset %d: %s", (int)erroroffset, buffer);
            return NULL;
        }
    }
    
    match_data = pcre2_match_data_create_from_pattern(re, NULL);
    
    total = 0;      // the length of string  after replacement
    offset = 0;
    option = 0;
    while((rc = pcre2_match(re, s1, len, offset, option, match_data, NULL)) > 0){
        ovector = pcre2_get_ovector_pointer(match_data);
        str = dl_list_push(list);
        str->data = s1 + offset;
        str->len = ovector[0] - offset;
        
        offset = ovector[1];
        
        total += str->len + len_3;
    }
    
    if(rc == PCRE2_ERROR_NOMATCH){
        str = dl_list_push(list);
        str->data = s1 + offset;
        str->len = len - offset;
        
        total += str->len;
    }else{
        dl_log_error(DL_LOG_ERR, p->log, "pcre2 unknown error");
        dl_destroy_pool(tmp_p);
        pcre2_match_data_free(match_data);
        if(rep == NULL) pcre2_code_free(re);
        
        return NULL;
    }
    
    res_s->data = dl_palloc(p, total);
    res_s->len = total;
    cur = res_s->data;
    
    for(part = &list->part; part; part = part->next){
        str = part->elts;
        
        cur = dl_cpymem(cur, str->data, str->len);
        
        if(part != list->last){
            cur = dl_cpymem(cur, s3, len_3);
        }
    }
    
    pcre2_match_data_free(match_data);
    dl_destroy_pool(tmp_p);
    
    if(rep == NULL) pcre2_code_free(re);
    
    return res_s;

}

/* match substring */

int
dl_str_match_re(char *s1, size_t len,
                char *pattern, pcre2_code *rep, dl_log *log)
{
    PCRE2_SIZE  erroroffset;
    PCRE2_SIZE  *ovector;
    int errornumber, rc, offset, option;
    pcre2_match_data    *match_data;
    pcre2_code          *re;
    
    re = rep;
    
    if(rep == NULL){
        re = pcre2_compile(pattern, PCRE2_ZERO_TERMINATED, 0, &errornumber, &erroroffset, NULL);
        if(re == NULL){
            if(log){
                PCRE2_UCHAR buffer[256];
                pcre2_get_error_message(errornumber, buffer, sizeof(buffer));
                dl_log_error(DL_LOG_ERR, log,
                            "PCRE2 compilation failed at offset %d: %s", (int)erroroffset, buffer);
            }
            return 0;
        }
    }
    
    match_data = pcre2_match_data_create_from_pattern(re, NULL);
    
    offset = 0;
    option = 0;
    rc = pcre2_match(re, s1, len, offset, option, match_data, NULL);
    
    pcre2_match_data_free(match_data);
    
    if(rep == NULL) pcre2_code_free(re);
    
    if(rc > 0)
        return rc;
    else
        return 0;
}

/*
    match and capture
*/
dl_list *
dl_str_capture_re(dl_pool *p, char *s1, size_t len,
                   char *pattern, pcre2_code *rep, int all)
{
    dl_str      *str;
    dl_list     *list, **lt;
    
    PCRE2_SIZE  erroroffset;
    PCRE2_SIZE  *ovector;
    int total, errornumber, rc, offset, option, i;
    pcre2_match_data    *match_data;
    pcre2_code          *re;
    
    re = rep;
    
    if(rep == NULL){
        re = pcre2_compile(pattern, PCRE2_ZERO_TERMINATED, 0, &errornumber, &erroroffset, NULL);
        if(re == NULL){
            PCRE2_UCHAR buffer[256];
            pcre2_get_error_message(errornumber, buffer, sizeof(buffer));
            dl_log_error(DL_LOG_ERR, p->log,
                        "PCRE2 compilation failed at offset %d: %s", (int)erroroffset, buffer);
            return NULL;
        }
    }
    
    match_data = pcre2_match_data_create_from_pattern(re, NULL);
    
    offset = 0;
    option = 0;
    
    if(all == 1){
        list = dl_list_create(p, 1, sizeof(void *));
        if(list == NULL) return NULL;
        
        while((rc = pcre2_match(re, s1, len, offset, option, match_data, NULL)) > 0){
            ovector = pcre2_get_ovector_pointer(match_data);
            
            lt = dl_list_push(list);
            *lt = dl_list_create(p, 1, sizeof(dl_str));
            
            for (i = 0; i < rc; i++){
                str = dl_list_push(*lt);
                str->data = s1 + ovector[2*i];
                str->len = ovector[2*i+1] - ovector[2*i];
            }

            offset = ovector[1];
        }
        
        if(rc != PCRE2_ERROR_NOMATCH){
            dl_log_error(DL_LOG_ERR, p->log, "pcre2 unknown error");
        }
        
    }else{
        list = dl_list_create(p, 1, sizeof(dl_str));
        if(list == NULL) return NULL;
        
        rc = pcre2_match(re, s1, len, offset, option, match_data, NULL);
        if(rc > 0){
            ovector = pcre2_get_ovector_pointer(match_data);
            
            for (i = 0; i < rc; i++){
                str = dl_list_push(list);
                str->data = s1 + ovector[2*i];
                str->len = ovector[2*i+1] - ovector[2*i];

                //dl_printf("%d--%V\n", i, str);
            }
        }else{
            if(rc != PCRE2_ERROR_NOMATCH){
                dl_log_error(DL_LOG_ERR, p->log, "pcre2 unknown error");
            }
        }
        
    }
    
    pcre2_match_data_free(match_data);
    
    if(rep == NULL) pcre2_code_free(re);
    
    return list;
}


