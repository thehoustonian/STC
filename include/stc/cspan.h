/* MIT License
 *
 * Copyright (c) 2023 Tyge Løvset
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
/*
#include <stdio.h>
#include <stc/cspan.h>
#include <stc/algo/filter.h>
using_cspan(Span2f, float, 2);
using_cspan(Intspan, int, 1);

int demo1() {
    float raw[4*5];
    Span2f ms = cspan_make(raw, 4, 5);
    
    for (size_t i=0; i<ms.dim[0]; i++)
        for (size_t j=0; j<ms.dim[1]; j++)
            *cspan_at(&ms, i, j) = i*1000 + j;

    printf("%f\n", *cspan_at(&ms, 3, 4));
}

int demo2() {
    int array[] = {1, 2, 3, 4, 5};
    Intspan span = cspan_from_array(array);
    
    c_FOREACH (i, Intspan, span)
        printf(" %d", *i.ref);
    puts("");
    
    // use a temporary Intspan object.
    c_FORFILTER (i, Intspan, cspan_literal(Intspan, {10, 20, 30, 23, 22, 21})
                  , c_FLT_SKIPWHILE(i, *i.ref < 25)
                 && (*i.ref & 1) == 0 // even only
                  , c_FLT_TAKE(i, 2)) // break after 2
        printf(" %d", *i.ref);
    puts("");
}
*/
#ifndef STC_CSPAN_H_INCLUDED
#define STC_CSPAN_H_INCLUDED

#include "ccommon.h"

#define using_cspan(Self, T, RANK) \
    typedef T Self##_value, Self##_raw; \
    typedef struct { Self##_value *ref, *end; } Self##_iter; \
    typedef struct { Self##_value *data; uint32_t dim[RANK]; } Self; \
    \
    STC_INLINE Self##_iter Self##_begin(const Self* self) { \
        Self##_iter it = {self->data, self->data + cspan_size(self)}; \
        return it; \
    } \
    STC_INLINE Self##_iter Self##_end(const Self* self) { \
        Self##_iter it = {NULL, self->data + cspan_size(self)}; \
        return it; \
    } \
    STC_INLINE void Self##_next(Self##_iter* it) \
        { if (++it->ref == it->end) it->ref = NULL; } \
    struct stc_nostruct

#define using_cspan2(Self, T) using_cspan(Self##1, T, 1); using_cspan(Self##2, T, 2)
#define using_cspan3(Self, T) using_cspan2(Self, T); using_cspan(Self##3, T, 3)
#define using_cspan4(Self, T) using_cspan3(Self, T); using_cspan(Self##4, T, 4)

#define cspan_rank_ok(self, rank) c_STATIC_ASSERT(cspan_rank(self) == rank)

#define cspan_literal(S, ...) \
    (c_INIT(S){.data = (S##_value[])__VA_ARGS__, \
               .dim = {sizeof((S##_value[])__VA_ARGS__)/sizeof(S##_value)}})

#define cspan_make(data, ...) \
    {data, {__VA_ARGS__}}

/* create a cspan from a cvec, cstack, cdeq, cqueue, or cpque (heap) */
#define cspan_from(container) \
    {(container)->data, {(container)->_len}}

#define cspan_from_array(array) \
    {(array) + c_STATIC_ASSERT(sizeof(array) != sizeof(void*)), {c_ARRAYLEN(array)}}

#define cspan_size(self) _cspan_size((self)->dim, cspan_rank(self))
#define cspan_rank(self) c_ARRAYLEN((self)->dim)
#define cspan_index(self, ...) \
    c_PASTE(_cspan_i, c_NUMARGS(__VA_ARGS__))((self)->dim, __VA_ARGS__) + \
                                              cspan_rank_ok(self, c_NUMARGS(__VA_ARGS__))

#define cspan_resize(self, ...) \
    (void)memcpy((self)->dim, (uint32_t[]){__VA_ARGS__}, \
                 sizeof((self)->dim) + cspan_rank_ok(self, c_NUMARGS(__VA_ARGS__)))

#define cspan_at(self, ...) ((self)->data + cspan_index(self, __VA_ARGS__))

#define cspan_slice1(self, x0, width) \
    {cspan_at(self, x0), {width}}
#define cspan_slice2(self, x0, width) \
    {cspan_at(self, x0, 0), {width, (self)->dim[1]}}
#define cspan_slice3(self, x0, width) \
    {cspan_at(self, x0, 0, 0), {width, (self)->dim[1], (self)->dim[2]}}
#define cspan_slice4(self, x0, width) \
    {cspan_at(self, x0, 0, 0, 0), {width, (self)->dim[1], (self)->dim[2], (self)->dim[3]}}

#define cspan_2to1(self, x) \
    {cspan_at(self, x, 0), {(self)->dim[1]}}
#define cspan_3to1(self, x, y) \
    {cspan_at(self, x, y, 0), {(self)->dim[2]}}
#define cspan_3to2(self, x) \
    {cspan_at(self, x, 0, 0), {(self)->dim[1], (self)->dim[2]}}
#define cspan_4to1(self, x, y, z) \
    {cspan_at(self, x, y, z, 0), {(self)->dim[3]}}
#define cspan_4to2(self, x, y) \
    {cspan_at(self, x, y, 0, 0), {(self)->dim[2], (self)->dim[3]}}
#define cspan_4to3(self, x) \
    {cspan_at(self, x, 0, 0, 0), {(self)->dim[1], (self)->dim[2], (self)->dim[3]}}

STC_INLINE size_t _cspan_i1(const uint32_t dim[1], uint32_t x)
    { c_ASSERT(x < dim[0]); return x; }

STC_INLINE size_t _cspan_i2(const uint32_t dim[2], uint32_t x, uint32_t y)
    { c_ASSERT(x < dim[0] && y < dim[1]); return dim[1]*x + y; }

STC_INLINE size_t _cspan_i3(const uint32_t dim[3], uint32_t x, uint32_t y, uint32_t z) {
    c_ASSERT(x < dim[0] && y < dim[1] && z < dim[2]);
    return dim[2]*(dim[1]*x + y) + z;
}
STC_INLINE size_t _cspan_i4(const uint32_t dim[4], uint32_t x, uint32_t y, uint32_t z, uint32_t w) {
    c_ASSERT(x < dim[0] && y < dim[1] && z < dim[3] && w < dim[3]);
    return dim[3]*(dim[2]*(dim[1]*x + y) + z) + w;
}
STC_INLINE size_t _cspan_size(const uint32_t dim[], unsigned rank) {
    size_t sz = dim[0];
    while (rank --> 1) sz *= dim[rank];
    return sz;
}

#endif
