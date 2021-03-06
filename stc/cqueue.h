/* MIT License
 *
 * Copyright (c) 2021 Tyge Løvset, NORCE, www.norceresearch.no
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
#ifndef CQUEUE_H_INCLUDED
#define CQUEUE_H_INCLUDED

/*  Queue adapter, default uses clist.

    #include <stc/crandom.h>
    #include <stc/cqueue.h>
    using_cdeq(i, int);
    using_cqueue(i, cdeq_i);

    int main() {
        int n = 10000000;
        stc64_t rng = stc64_init(1234);
        stc64_uniform_t dist = stc64_uniform_init(rng, 0, n);

        cqueue_i queue = cqueue_i_init();

        // Push ten million random numbers onto the queue.
        for (int i=0; i<n; ++i)
            cqueue_i_push(&queue, stc64_uniform(&dist));

        // Push or pop on the queue ten million times
        printf("%d\n", n);
        for (int i=n; i>0; --i) {
            int r = stc64_uniform(&dist);
            if (r & 1)
                ++n, cqueue_i_push(&queue, r);
            else
                --n, cqueue_i_pop(&queue);
        }
        printf("%d\n", n);
        cqueue_i_del(&queue);
    }
*/
#include "cdeq.h"

#define using_cqueue(X, ctype) \
    typedef ctype##_t cqueue_##X; \
    typedef ctype##_value_t cqueue_##X##_value_t; \
    typedef ctype##_rawvalue_t cqueue_##X##_rawvalue_t; \
\
    STC_INLINE cqueue_##X \
    cqueue_##X##_init(void) {return ctype##_init();} \
    STC_INLINE cqueue_##X \
    cqueue_##X##_clone(cqueue_##X q) {return ctype##_clone(q);} \
    STC_INLINE cqueue_##X##_value_t \
    cqueue_##X##_value_clone(cqueue_##X##_value_t val) {return ctype##_value_clone(val);} \
    STC_INLINE void \
    cqueue_##X##_del(cqueue_##X* self) {ctype##_del(self);} \
    STC_INLINE size_t \
    cqueue_##X##_size(cqueue_##X q) {return ctype##_size(q);} \
    STC_INLINE bool \
    cqueue_##X##_empty(cqueue_##X q) {return ctype##_empty(q);} \
    STC_INLINE cqueue_##X##_value_t* \
    cqueue_##X##_front(const cqueue_##X* self) {return ctype##_front(self);} \
    STC_INLINE cqueue_##X##_value_t* \
    cqueue_##X##_back(const cqueue_##X* self) {return ctype##_back(self);} \
    STC_INLINE void \
    cqueue_##X##_pop(cqueue_##X* self) {ctype##_pop_front(self);} \
    STC_INLINE void \
    cqueue_##X##_push(cqueue_##X* self, ctype##_value_t value) { \
        ctype##_push_back(self, value); \
    } \
    STC_INLINE void \
    cqueue_##X##_emplace(cqueue_##X* self, cqueue_##X##_rawvalue_t raw) { \
        ctype##_emplace_back(self, raw); \
    } \
    STC_INLINE void \
    cqueue_##X##_emplace_n(cqueue_##X *self, const cqueue_##X##_rawvalue_t arr[], size_t size) { \
        ctype##_emplace_n(self, arr, size); \
    } \
    typedef ctype##_iter_t cqueue_##X##_iter_t; \
    STC_INLINE cqueue_##X##_iter_t \
    cqueue_##X##_begin(const cqueue_##X* self) {return ctype##_begin(self);} \
    STC_INLINE cqueue_##X##_iter_t \
    cqueue_##X##_end(const cqueue_##X* self) {return ctype##_end(self);} \
    STC_INLINE void \
    cqueue_##X##_next(cqueue_##X##_iter_t* it) {ctype##_next(it);} \
    STC_INLINE cqueue_##X##_value_t* \
    cqueue_##X##_itval(cqueue_##X##_iter_t it) {return ctype##_itval(it);} \
\
    typedef cqueue_##X cqueue_##X##_t

#endif
