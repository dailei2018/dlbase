#include "dl_base.h"
#include "t_base.h"

int main(){
    dl_log  *log = dl_log_init(DL_LOG_NOTICE, "/tmp/test.log");
    if(log == NULL) return -1;
    
    dl_log_error(DL_LOG_NOTICE, log, "notice log");
    
    dl_log_error(DL_LOG_NOTICE, log, "notice log22222");
    
    dl_log_error(DL_LOG_DEBUG, log, "debug....");
    
    dl_log_free(log);
    
    return 0;
}