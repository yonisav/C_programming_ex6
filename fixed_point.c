#include "fixed_point.h"
#include <stdio.h>
#include <stdint.h>

void print_fixed(int16_t raw, int16_t q) {

    // Print up to 6 decimal places (with truncation)

    double val = (double)raw / (1 << q); // val = raw / q^2
    double truncated = (int64_t)(val * 1000000) / 1000000.0;

    printf("%.6f", truncated);
}

int16_t add_fixed(int16_t a, int16_t b) {
    // Implement addition
    return a + b;
}

int16_t subtract_fixed(int16_t a, int16_t b) {
    // TImplement subtraction
    return a - b;
}

int16_t multiply_fixed(int16_t a, int16_t b, int16_t q) {
    //Implement fixed-point multiply:
    //- Use a wider type for intermediate multiplication (e.g., int32_t or int64_t).
    int64_t temp = (int64_t)a * b;
    return(int16_t)(temp >> q);
}

void eval_poly_ax2_minus_bx_plus_c_fixed(int16_t x, int16_t a, int16_t b, int16_t c, int16_t q) {
    /* TODO:
       Evaluate: y = a*x^2 - b*x + c
    */

    printf("the polynomial output for a=");
    print_fixed(a, q);
    printf(", b=");
    print_fixed(b, q);
    printf(", c=");
    print_fixed(c, q);
    printf(" is ");
    /* TODO: print y once computed */
    /* print_fixed(y, q); */

}
