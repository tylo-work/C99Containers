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
#ifndef CLIST_H_INCLUDED
#define CLIST_H_INCLUDED

/*  Circular Singly-linked Lists.

    This implements a std::forward_list-like class in C, but because it is circular,
    it also support push* and splice* at both ends of the list. This makes it ideal
    for being used as a queue, unlike std::forward_list. Basic usage is similar to cvec:

    #include <stdio.h>
    #include <stc/clist.h>
    #include <stc/crandom.h>
    using_clist(ix, int64_t);

    int main() {
        clist_ix list = clist_ix_init();
        stc64_t rng = stc64_init(12345);
        int n;
        for (int i=0; i<1000000; ++i) // one million
            clist_ix_push_back(&list, stc64_rand(&rng) >> 32);
        n = 0;
        c_foreach (i, clist_ix, list)
            if (++n % 10000 == 0) printf("%8d: %10zd\n", n, i.ref->value);
        // Sort them...
        clist_ix_sort(&list); // mergesort O(n*log n)
        n = 0;
        puts("sorted");
        c_foreach (i, clist_ix, list)
            if (++n % 10000 == 0) printf("%8d: %10zd\n", n, i.ref->value);
        clist_ix_del(&list);
    }
*/
#include "ccommon.h"
#include <stdlib.h>

#define using_clist(...)   c_MACRO_OVERLOAD(using_clist, __VA_ARGS__)
#define using_clist_2(X, Value) \
                    using_clist_3(X, Value, c_default_compare)
#define using_clist_3(X, Value, valueCompare) \
                    using_clist_7(X, Value, valueCompare, c_trivial_del, c_trivial_fromraw, c_trivial_toraw, Value)
#define using_clist_5(X, Value, valueCompare, valueDel, valueClone) \
                    using_clist_7(X, Value, valueCompare, valueDel, valueClone, c_trivial_toraw, Value)
#define using_clist_str() \
                    using_clist_7(str, cstr_t, cstr_compare_raw, cstr_del, cstr_from, cstr_c_str, const char*)

#define using_clist_types(X, Value) \
    typedef Value clist_##X##_value_t; \
\
    typedef struct clist_##X##_node { \
        struct clist_##X##_node* next; \
        clist_##X##_value_t value; \
    } clist_##X##_node_t; \
\
    typedef struct { \
        clist_##X##_node_t* last; \
    } clist_##X; \
\
    typedef struct { \
        clist_##X##_node_t* const* _last; \
        clist_##X##_value_t* ref; \
        int _state; \
    } clist_##X##_iter_t


using_clist_types(void, int);
STC_API size_t _clist_size(const clist_void* self);
#define _clist_node(X, vp) c_container_of(vp, clist_##X##_node_t, value)

#define using_clist_7(X, Value, valueCompareRaw, valueDel, valueFromRaw, valueToRaw, RawValue) \
\
    using_clist_types(X, Value); \
    typedef RawValue clist_##X##_rawvalue_t; \
\
    STC_INLINE clist_##X \
    clist_##X##_init(void) {clist_##X x = {NULL}; return x;} \
    STC_INLINE bool \
    clist_##X##_empty(clist_##X ls) {return ls.last == NULL;} \
    STC_INLINE size_t \
    clist_##X##_size(clist_##X ls) {return _clist_size((const clist_void*) &ls);} \
    STC_INLINE Value \
    clist_##X##_value_fromraw(RawValue raw) {return valueFromRaw(raw);} \
    STC_INLINE clist_##X##_value_t \
    clist_##X##_value_clone(clist_##X##_value_t val) {return valueFromRaw(valueToRaw(&val));} \
\
    STC_API void \
    clist_##X##_del(clist_##X* self); \
    STC_API clist_##X \
    clist_##X##_clone(clist_##X list); \
    STC_INLINE void \
    clist_##X##_clear(clist_##X* self) {clist_##X##_del(self);} \
\
    STC_API void \
    clist_##X##_emplace_n(clist_##X *self, const clist_##X##_rawvalue_t arr[], size_t size); \
    STC_API void \
    clist_##X##_push_back(clist_##X* self, Value value); \
    STC_INLINE void \
    clist_##X##_emplace_back(clist_##X* self, RawValue raw) { \
        clist_##X##_push_back(self, valueFromRaw(raw)); \
    } \
    STC_API void \
    clist_##X##_push_front(clist_##X* self, Value value); \
    STC_INLINE void \
    clist_##X##_emplace_front(clist_##X* self, RawValue raw) { \
        clist_##X##_push_front(self, valueFromRaw(raw)); \
    } \
\
    STC_API clist_##X##_node_t* \
    _clist_##X##_erase_after(clist_##X* self, clist_##X##_node_t* node); \
    STC_INLINE void \
    clist_##X##_pop_front(clist_##X* self) { \
        _clist_##X##_erase_after(self, self->last); \
    } \
    STC_INLINE clist_##X##_iter_t \
    clist_##X##_before_begin(const clist_##X* self) { \
        clist_##X##_value_t *last = self->last ? &self->last->value : NULL; \
        clist_##X##_iter_t it = {&self->last, last, -1}; return it; \
    } \
    STC_INLINE clist_##X##_iter_t \
    clist_##X##_begin(const clist_##X* self) { \
        clist_##X##_value_t* head = self->last ? &self->last->next->value : NULL; \
        clist_##X##_iter_t it = {&self->last, head, 0}; return it; \
    } \
    STC_INLINE clist_##X##_iter_t \
    clist_##X##_last(const clist_##X* self) { \
        clist_##X##_value_t *last = self->last ? &self->last->value : NULL; \
        clist_##X##_iter_t it = {&self->last, last, 0}; return it; \
    } \
    STC_INLINE clist_##X##_iter_t \
    clist_##X##_end(const clist_##X* self) { \
        clist_##X##_iter_t it = {NULL, NULL}; return it; \
    } \
    STC_INLINE void \
    clist_##X##_next(clist_##X##_iter_t* it) { \
        clist_##X##_node_t* node = _clist_node(X, it->ref); \
        it->ref = ((it->_state += node == *it->_last) == 1) ? NULL : &node->next->value; \
    } \
    STC_INLINE clist_##X##_iter_t \
    clist_##X##_fwd(clist_##X##_iter_t it, size_t n) { \
        c_forrange_1 (n) clist_##X##_next(&it); return it; \
    } \
    STC_INLINE clist_##X##_value_t* \
    clist_##X##_itval(clist_##X##_iter_t it) {return it.ref;} \
    \
    STC_API clist_##X##_iter_t \
    clist_##X##_insert_after(clist_##X* self, clist_##X##_iter_t pos, Value value); \
    STC_INLINE clist_##X##_iter_t \
    clist_##X##_emplace_after(clist_##X* self, clist_##X##_iter_t pos, RawValue raw) { \
        return clist_##X##_insert_after(self, pos, valueFromRaw(raw)); \
    } \
    STC_API clist_##X##_iter_t \
    clist_##X##_erase_after(clist_##X* self, clist_##X##_iter_t pos); \
    STC_API clist_##X##_iter_t \
    clist_##X##_erase_range_after(clist_##X* self, clist_##X##_iter_t pos, clist_##X##_iter_t finish); \
\
    STC_API clist_##X##_iter_t \
    clist_##X##_splice_after(clist_##X* self, clist_##X##_iter_t pos, clist_##X* other); \
    STC_INLINE clist_##X##_iter_t \
    clist_##X##_splice_front(clist_##X* self, clist_##X* other) { \
        return clist_##X##_splice_after(self, clist_##X##_before_begin(self), other); \
    } \
    STC_INLINE clist_##X##_iter_t \
    clist_##X##_splice_back(clist_##X* self, clist_##X* other) { \
        return clist_##X##_splice_after(self, clist_##X##_last(self), other); \
    } \
\
    STC_API clist_##X##_iter_t \
    clist_##X##_find_before_in_range(const clist_##X* self, clist_##X##_iter_t first, clist_##X##_iter_t finish, RawValue val); \
    STC_API clist_##X##_iter_t \
    clist_##X##_find_before(const clist_##X* self, RawValue val); \
    STC_API clist_##X##_iter_t \
    clist_##X##_find(const clist_##X* self, RawValue val); \
    STC_API size_t \
    clist_##X##_remove(clist_##X* self, RawValue val); \
    STC_API void \
    clist_##X##_sort(clist_##X* self); \
\
    STC_INLINE Value* \
    clist_##X##_front(const clist_##X* self) {return &self->last->next->value;} \
    STC_INLINE Value* \
    clist_##X##_back(const clist_##X* self) {return &self->last->value;} \
\
    _c_implement_clist_7(X, Value, valueCompareRaw, valueDel, valueFromRaw, valueToRaw, RawValue) \
    typedef clist_##X clist_##X##_t

/* -------------------------- IMPLEMENTATION ------------------------- */

#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION)
#define _c_implement_clist_7(X, Value, valueCompareRaw, valueDel, valueFromRaw, valueToRaw, RawValue) \
\
    STC_DEF clist_##X \
    clist_##X##_clone(clist_##X list) { \
        clist_##X out = clist_##X##_init(); \
        c_foreach_3 (i, clist_##X, list) \
            clist_##X##_emplace_back(&out, valueToRaw(i.ref)); \
        return out; \
    } \
    STC_DEF void \
    clist_##X##_del(clist_##X* self) { \
        while (self->last) _clist_##X##_erase_after(self, self->last); \
    } \
\
    STC_DEF void \
    clist_##X##_push_back(clist_##X* self, Value value) { \
        _c_clist_insert_after(self, X, self->last, value); \
        self->last = entry; \
    } \
    STC_DEF void \
    clist_##X##_push_front(clist_##X* self, Value value) { \
        _c_clist_insert_after(self, X, self->last, value); \
        if (!self->last) self->last = entry; \
    } \
    STC_DEF void \
    clist_##X##_emplace_n(clist_##X *self, const clist_##X##_rawvalue_t arr[], size_t size) { \
        for (size_t i=0; i<size; ++i) clist_##X##_push_back(self, valueFromRaw(arr[i])); \
    } \
\
    STC_DEF clist_##X##_iter_t \
    clist_##X##_insert_after(clist_##X* self, clist_##X##_iter_t pos, Value value) { \
        clist_##X##_node_t* node = pos.ref ? _clist_node(X, pos.ref) : NULL; \
        _c_clist_insert_after(self, X, node, value); \
        if (!node || node == self->last && pos._state == 0) self->last = entry; \
        pos.ref = &entry->value, pos._state = 0; return pos; \
    } \
    STC_DEF clist_##X##_iter_t \
    clist_##X##_erase_after(clist_##X* self, clist_##X##_iter_t pos) { \
        _clist_##X##_erase_after(self, _clist_node(X, pos.ref)); \
        clist_##X##_next(&pos); return pos; \
    } \
    STC_DEF clist_##X##_iter_t \
    clist_##X##_erase_range_after(clist_##X* self, clist_##X##_iter_t first, clist_##X##_iter_t finish) { \
        clist_##X##_node_t* node = _clist_node(X, first.ref), *done = finish.ref ? _clist_node(X, finish.ref) : NULL; \
        while (node && node->next != done) \
            node = _clist_##X##_erase_after(self, node); \
        clist_##X##_next(&first); return first; \
    } \
\
    STC_DEF clist_##X##_iter_t \
    clist_##X##_find_before_in_range(const clist_##X* self, clist_##X##_iter_t first, clist_##X##_iter_t finish, RawValue val) { \
        clist_##X##_iter_t i = first; \
        for (clist_##X##_next(&i); i.ref != finish.ref; clist_##X##_next(&i)) { \
            RawValue r = valueToRaw(i.ref); \
            if (valueCompareRaw(&r, &val) == 0) return first; \
            first = i; \
        } \
        return clist_##X##_end(self); \
    } \
    STC_DEF clist_##X##_iter_t \
    clist_##X##_find_before(const clist_##X* self, RawValue val) { \
        clist_##X##_iter_t it = clist_##X##_find_before_in_range(self, clist_##X##_before_begin(self), clist_##X##_end(self), val); \
        return it; \
    } \
    STC_DEF clist_##X##_iter_t \
    clist_##X##_find(const clist_##X* self, RawValue val) { \
        clist_##X##_iter_t it = clist_##X##_find_before_in_range(self, clist_##X##_before_begin(self), clist_##X##_end(self), val); \
        if (it.ref != clist_##X##_end(self).ref) clist_##X##_next(&it); \
        return it; \
    } \
\
    STC_DEF clist_##X##_node_t* \
    _clist_##X##_erase_after(clist_##X* self, clist_##X##_node_t* node) { \
        clist_##X##_node_t* del = node->next, *next = del->next; \
        node->next = next; \
        if (del == next) self->last = node = NULL; \
        else if (self->last == del) self->last = node, node = NULL; \
        valueDel(&del->value); c_free(del); \
        return node; \
    } \
\
    STC_DEF size_t \
    clist_##X##_remove(clist_##X* self, RawValue val) { \
        size_t n = 0; \
        clist_##X##_node_t* prev = self->last, *node; \
        while (prev) { \
            node = prev->next; \
            RawValue r = valueToRaw(&node->value); \
            if (valueCompareRaw(&r, &val) == 0) \
                prev = _clist_##X##_erase_after(self, prev), ++n; \
            else \
                prev = (node == self->last ? NULL : node); \
        } \
        return n; \
    } \
\
    STC_DEF clist_##X##_iter_t \
    clist_##X##_splice_after(clist_##X* self, clist_##X##_iter_t pos, clist_##X* other) { \
        clist_##X##_iter_t it = clist_##X##_last(other); \
        if (!pos.ref) \
            self->last = other->last; \
        else if (other->last) { \
            clist_##X##_node_t *node = _clist_node(X, pos.ref), *next = node->next; \
            node->next = other->last->next; \
            other->last->next = next; \
            if (node == self->last && pos._state == 0) self->last = other->last; \
        } \
        other->last = NULL; \
        it._last = &self->last; return it; \
    } \
\
    STC_DEF clist_##X \
    clist_##X##_splice_out(clist_##X* self, clist_##X##_iter_t pos1, clist_##X##_iter_t pos2) { \
        clist_##X##_node_t *node1 = _clist_node(X, pos1.ref), *next1 = node1->next, \
                           *node2 = _clist_node(X, pos2.ref); \
        node1->next = node2->next, node2->next = next1; \
        if (self->last == node2) self->last = node1; \
        clist_##X list = {node2}; return list; \
    } \
\
    STC_INLINE int \
    clist_##X##_sort_compare(const void* x, const void* y) { \
        RawValue a = valueToRaw(&((clist_##X##_node_t *) x)->value); \
        RawValue b = valueToRaw(&((clist_##X##_node_t *) y)->value); \
        return valueCompareRaw(&a, &b); \
    } \
    STC_DEF void \
    clist_##X##_sort(clist_##X* self) { \
        clist_void_node_t* last = _clist_mergesort((clist_void_node_t *) self->last->next, clist_##X##_sort_compare); \
        self->last = (clist_##X##_node_t *) last; \
    }


#define _c_clist_insert_after(self, X, node, val) \
    clist_##X##_node_t *entry = c_new_1 (clist_##X##_node_t), \
                       *next = self->last ? node->next : entry; \
    entry->value = val; \
    entry->next = next; \
    if (node) node->next = entry
    /* +: set self->last based on node */

STC_DEF size_t
_clist_size(const clist_void* self) {
    const clist_void_node_t *i = self->last;
    if (!i) return 0;
    size_t n = 1;
    while ((i = i->next) != self->last) ++n;
    return n;
}

/* Singly linked list Mergesort implementation by Simon Tatham. O(n*log n).
 * https://www.chiark.greenend.org.uk/~sgtatham/algorithms/listsort.html
 */
STC_DEF clist_void_node_t *
_clist_mergesort(clist_void_node_t *list, int (*cmp)(const void*, const void*)) {
    clist_void_node_t *p, *q, *e, *tail, *oldhead;
    int insize = 1, nmerges, psize, qsize, i;
    if (!list) return NULL;

    while (1) {
        p = oldhead = list;
        list = tail = NULL;
        nmerges = 0;

        while (p) {
            ++nmerges;
            q = p, psize = 0;
            for (i = 0; i < insize; ++i) {
                ++psize;
                q = (q->next == oldhead ? NULL : q->next);
                if (!q) break;
            }
            qsize = insize;

            while (psize > 0 || (qsize > 0 && q)) {
                if (psize == 0) {
                    e = q, q = q->next, --qsize;
                    if (q == oldhead) q = NULL;
                } else if (qsize == 0 || !q) {
                    e = p, p = p->next, --psize;
                    if (p == oldhead) p = NULL;
                } else if (cmp(p, q) <= 0) {
                    e = p, p = p->next, --psize;
                    if (p == oldhead) p = NULL;
                } else {
                    e = q, q = q->next, --qsize;
                    if (q == oldhead) q = NULL;
                }
                if (tail) tail->next = e; else list = e;
                tail = e;
            }
            p = q;
        }
        tail->next = list;

        if (nmerges <= 1)
            return tail;

        insize *= 2;
    }
}

#else
#define _c_implement_clist_7(X, Value, valueCompareRaw, valueDel, valueFromRaw, valueToRaw, RawValue)
#endif

#endif
