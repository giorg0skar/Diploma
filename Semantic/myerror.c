#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <dlfcn.h>

extern int linenumber;

// void error (const char * fmt, ...)
// {
//    va_list ap;

//    va_start(ap, fmt);
//    if (fmt[0] == '\r')
//       fmt++;
// //    else
// //       fprintf(stderr, "%s:%d: ", filename, linecount);
//       //fprintf(stderr, "%s:%d: ", filename, linenumber);
//    fprintf(stderr, "Error, ");
//    vfprintf(stderr, fmt, ap);
//    fprintf(stderr, "\n");
//    va_end(ap);
//    fprintf(stderr, "The dana compiler is lazy and aborts...\n");
//    exit(1);
// }


//redefine exit function
// extern void exit(int __status) {
//     printf("error has happened\n");
//     extern void (*original_exit)(int);
//     original_exit = dlsym(RTLD_NEXT, "exit");
//     return (*original_exit)(__status);
//     //__THROW __attribute__ ((__noreturn__));
// }
