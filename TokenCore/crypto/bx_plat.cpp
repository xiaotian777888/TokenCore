#include "bx_plat.h"

#ifdef BX_OS_UNIX_LIKE


int sprintf_s(char *__s, size_t __s_size, const char *__format, ...) {
    va_list valist;
    va_start(valist, __format);
    vsprintf(__s, __format, valist);
    va_end(valist);

    return 0;
}

//char *strupr_s(char *__s, size_t __s_size) {
//    char *ptr = __s;
//
//    while (*ptr != '\0') {
//        if (islower(*ptr))
//            *ptr = toupper(*ptr);
//        ptr++;
//    }
//
//    return __s;
//}

void rand_s(unsigned int *piRand) {
    if (NULL == piRand)
        return;

    *piRand = (unsigned int) rand();
}


#endif // #ifdef BX_OS_UNIX_LIKE
