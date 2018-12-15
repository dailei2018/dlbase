//#define NO_OUTPUT

void
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
