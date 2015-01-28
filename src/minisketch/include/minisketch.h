#ifndef _MINISKETCH_H_
#define _MINISKETCH_H_ 1

#include <stdint.h>
#include <stdlib.h>

#ifdef _MSC_VER
#  include <BaseTsd.h>
   typedef SSIZE_T ssize_t;
#else
#  include <unistd.h>
#endif

#ifndef MINISKETCH_API
# if defined(_WIN32)
#  ifdef MINISKETCH_BUILD
#   define MINISKETCH_API __declspec(dllexport)
#  else
#   define MINISKETCH_API
#  endif
# elif defined(__GNUC__) && (__GNUC__ >= 4) && defined(MINISKETCH_BUILD)
#  define MINISKETCH_API __attribute__ ((visibility ("default")))
# else
#  define MINISKETCH_API
# endif
#endif

#ifdef __cplusplus
#  if __cplusplus >= 201103L
#    include <memory>
#    include <vector>
#    include <cassert>
#    if __cplusplus >= 201703L
#      include <optional>
#    endif // __cplusplus >= 201703L
#  endif // __cplusplus >= 201103L
