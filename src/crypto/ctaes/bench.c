#include <stdio.h>
#include <math.h>
#include "sys/time.h"

#include "ctaes.h"

static double gettimedouble(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_usec * 0.000001 + tv.tv_sec;
}

static void print_number(double x) {
    double y = x;
    int c = 0;
    if (y < 0.0) {
        y = -y;
