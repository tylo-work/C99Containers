#include <stc/cstr.h>
#include <stc/cmap.h>
#include <stc/clist.h>
#include <stc/carray.h>

void check_del(float* v) {printf("destroy %g\n", *v);}

using_carray(f, float, check_del, c_trivial_fromraw);
using_clist(a, carray2f, c_no_compare, carray2f_del, c_no_clone);
using_cmap(l, int, clist_a, c_default_equals, c_default_hash, clist_a_del, c_no_clone);
using_cmap_strkey(s, cmap_l, cmap_l_del, c_no_clone);

int main() {
    int xdim = 4, ydim = 6;
    int x = 1, y = 5, tableKey = 42;
    const char* strKey = "first";
    cmap_l listMap = cmap_l_init();

    cmap_s myMap = cmap_s_init();

    // Construct.
    carray2f arr_a = carray2f_init(ydim, xdim, 0.f);
    printf("arr_a: (%zu, %zu)\n", carray2f_ydim(arr_a), carray2f_xdim(arr_a));

    clist_a tableList = clist_a_init();
    // Put in some data.
    *carray2f_at(&arr_a, y, x) = 3.1415927f; // aa[y][x]
    clist_a_push_back(&tableList, arr_a);
    cmap_l_insert(&listMap, tableKey, tableList);
    cmap_s_insert(&myMap, cstr_from(strKey), listMap);

    // Access the data entry
    carray2f arr_b = *clist_a_back(&cmap_l_find(&cmap_s_find(&myMap, strKey).ref->second, tableKey).ref->second);
    printf("value (%d, %d) is: %f\n", y, x, *carray2f_at(&arr_b, y, x));

    cmap_s_del(&myMap); // free up everything!
}
