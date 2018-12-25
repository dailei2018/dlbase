//#define NO_OUTPUT

static void
dl_printf(const char *format, ...)
{
#ifndef NO_OUTPUT
    char buf[1024];
    va_list argList;
    va_start(argList, format);

    vsnprintf(buf, 1024, format, argList);

    printf("%s", buf);
#endif

}

static void err_msg(const char *format, ...){
    char buf[1024];
    va_list argList;
    va_start(argList, format);

    vsnprintf(buf, 1024, format, argList);

    puts(buf);
    exit(1);
}
