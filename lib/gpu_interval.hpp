#pragma once

#include <math_constants.h>

struct Interval {
    float lower;
    float upper;

    __device__ inline Interval operator-() const {
        return {-upper, -lower};
    }
};

#ifdef __CUDACC__
__device__ inline Interval operator+(const Interval& x, const Interval& y) {
    return {__fadd_rd(x.lower, y.lower), __fadd_ru(x.upper, y.upper)};
}

__device__ inline Interval operator*(const Interval& x, const Interval& y) {
    if (x.lower < 0.0f) {
        if (x.upper > 0.0f) {
            if (y.lower < 0.0f) {
                if (y.upper > 0.0f) { // M * M
                    return {fminf(__fmul_rd(x.lower, y.upper),
                                  __fmul_rd(x.upper, y.lower)),
                            fmaxf(__fmul_ru(x.lower, y.lower),
                                  __fmul_ru(x.upper, y.upper))};
                } else { // M * N
                    return {__fmul_rd(x.upper, y.lower),
                            __fmul_ru(x.lower, y.lower)};
                }
            } else {
                if (y.upper > 0.0f) { // M * P
                    return {__fmul_rd(x.lower, y.upper),
                            __fmul_ru(x.upper, y.upper)};
                } else { // M * Z
                    return {0.0f, 0.0f};
                }
            }
        } else {
            if (y.lower < 0.0f) {
                if (y.upper > 0.0f) { // N * M
                    return {__fmul_rd(x.lower, y.upper),
                            __fmul_ru(x.lower, y.lower)};
                } else { // N * N
                    return {__fmul_rd(x.upper, y.upper),
                            __fmul_ru(x.lower, y.lower)};
                }
            } else {
                if (y.upper > 0.0f) { // N * P
                    return {__fmul_rd(x.lower, y.upper),
                            __fmul_ru(x.upper, y.lower)};
                } else { // N * Z
                    return {0.0f, 0.0f};
                }
            }
        }
    } else {
        if (x.upper > 0.0f) {
            if (y.lower < 0.0f) {
                if (y.upper > 0.0f) { // P * M
                    return {__fmul_rd(x.upper, y.lower),
                            __fmul_ru(x.upper, y.upper)};
                } else {// P * N
                    return {__fmul_rd(x.upper, y.lower),
                            __fmul_ru(x.lower, y.upper)};
                }
            } else {
                if (y.upper > 0.0f) { // P * P
                    return {__fmul_rd(x.lower, y.lower),
                            __fmul_ru(x.upper, y.upper)};
                } else {// P * Z
                    return {0.0f, 0.0f};
                }
            }
        } else { // Z * ?
            return {0.0f, 0.0f};
        }
    }
}

__device__ inline Interval operator/(const Interval& x, const Interval& y) {
    if (y.lower <= 0.0f && y.upper >= 0.0f) {
        return {-CUDART_INF_F, CUDART_INF_F};
    } else if (x.upper < 0.0f) {
        if (y.upper < 0.0f) {
            return { __fdiv_rd(x.upper, y.lower),
                     __fdiv_ru(x.lower, y.upper) };
        } else {
            return { __fdiv_rd(x.lower, y.lower),
                     __fdiv_ru(x.upper, y.upper) };
        }
    } else if (x.lower < 0.0f) {
        if (y.upper < 0.0f) {
            return { __fdiv_rd(x.upper, y.upper),
                     __fdiv_ru(x.lower, y.upper) };
        } else {
            return { __fdiv_rd(x.lower, y.lower),
                     __fdiv_ru(x.upper, y.lower) };
        }
    } else {
        if (y.upper < 0.0f) {
            return { __fdiv_rd(x.upper, y.upper),
                     __fdiv_ru(x.lower, y.lower) };
        } else {
            return { __fdiv_rd(x.lower, y.upper),
                     __fdiv_ru(x.upper, y.lower) };
        }
    }
}

__device__ inline Interval min(const Interval& x, const Interval& y) {
    return {fminf(x.lower, y.lower), fminf(x.upper, y.upper)};
}

__device__ inline Interval max(const Interval& x, const Interval& y) {
    return {fmaxf(x.lower, y.lower), fmaxf(x.upper, y.upper)};
}


__device__ inline Interval square(const Interval& x) {
    if (x.upper < 0.0f) {
        return {__fmul_rd(x.upper, x.upper), __fmul_ru(x.lower, x.lower)};
    } else if (x.lower > 0.0f) {
        return {__fmul_rd(x.lower, x.lower), __fmul_ru(x.upper, x.upper)};
    } else if (-x.lower > x.upper) {
        return {0.0f, __fmul_ru(x.lower, x.lower)};
    } else {
        return {0.0f, __fmul_ru(x.upper, x.upper)};
    }
}

__device__ inline Interval operator-(const Interval& x, const Interval& y) {
    return {__fsub_rd(x.lower, y.upper), __fsub_ru(x.upper, y.lower)};
}

__device__ inline Interval sqrt(const Interval& x) {
    if (x.upper < 0.0f) {
        return {CUDART_NAN_F, CUDART_NAN_F};
    } else if (x.lower <= 0.0f) {
        return {0.0f, __fsqrt_ru(x.upper)};
    } else {
        return {__fsqrt_rd(x.lower), __fsqrt_ru(x.upper)};
    }
}
#endif
