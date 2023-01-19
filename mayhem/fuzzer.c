#define CVECTOR_LOGARITHMIC_GROWTH
#include "../cvector.h"
#include "../cvector_utils.h"
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>


// Convert to char *
char *convert(uint8_t *a)
{
  char* buffer2;
  int i;

  buffer2 = malloc(9);
  if (!buffer2)
    return NULL;

  buffer2[8] = 0;
  for (i = 0; i <= 7; i++)
    buffer2[7 - i] = (((*a) >> i) & (0x01)) + '0';

  puts(buffer2);

  return buffer2;
}

int LLVMFuzzerTestOneInput(uint8_t *data, size_t size) {

    printf("============\n");
    int s_data = (size_t) data;
    
    
    cvector_vector_type(int) v           = NULL;
    cvector_vector_type(int) a           = NULL;
    cvector_vector_type(int) b           = NULL;
    cvector_vector_type(int) c           = NULL;
    cvector_vector_type(char *) str_vect = NULL;
    
    volatile int random;

    /* add some elements to the back */
    cvector_push_back(v, s_data);
    cvector_push_back(v, random);
   

    /* and remove one too */
    cvector_pop_back(v);

    for (unsigned int i = 0; i < size; i++) {
            volatile int anotherStackValue;
    	    cvector_push_back(v, anotherStackValue << i);
    }    

    printf("capacity: %zu\n", cvector_capacity(v));
    
    printf("size    : %zu\n", cvector_size(v));


    /* iterator over the vector using "iterator" style */
    if (v) {
    	int i = 0;
        int *it;
        for (it = cvector_begin(v); it != cvector_end(v); ++it) {
            printf("v[%d] = %d\n", i, *it);
	    i++;
        }
    }

    /* iterator over the vector standard indexing too! */
    if (v) {
        size_t i;
        for (i = 0; i < cvector_size(v); ++i) {
            printf("v[%zu] = %d\n", i, v[i]);
        }
    }

    /* well, we don't have destructors, so let's clean things up */
    cvector_free(v);

    putchar('\n');

    cvector_push_back(a, s_data);
    cvector_push_back(a, 1);
    cvector_push_back(a, 5);
    cvector_push_back(a, 4);
    cvector_pop_back(a); // delete 4
    cvector_push_back(a, 5);
    cvector_erase(a, 1); // delete 5
    cvector_erase(a, 0);
    cvector_insert(a, 0, 1);

    printf("a capacity: %zu\n", cvector_capacity(a));

    printf("a size    : %zu\n", cvector_size(a));


    if (a) {
        size_t i;
        cvector_copy(a, b);
        for (i = 0; i < cvector_size(b); ++i) {
            printf("a[%zu] = %d\n", i, a[i]);
        }
    }

    cvector_free(a);

    printf("After copy:\n");
    printf("b capacity: %zu\n", cvector_capacity(b));

    printf("b size    : %zu\n", cvector_size(b));

    putchar('\n');

    if (b) {
        size_t i;
        cvector_insert(b, 0, 0);
        cvector_insert(b, 2, 4);
        cvector_insert(b, 2, 2);
        cvector_insert(b, 3, 3);
        printf("b capacity: %zu\n", cvector_capacity(b));

        printf("b size    : %zu\n", cvector_size(b));

        // expected vector: [0, 1, 2, 3, 4, 5]
        for (i = 0; i < cvector_size(b); ++i) {
            printf("b[%zu] = %d\n", i, b[i]);

        }
    }

    cvector_free(b);

    cvector_reserve(c, 100);

    printf("c capacity: %zu\n", cvector_capacity(c));
    printf("c size        : %zu\n", cvector_size(c));
    cvector_push_back(c, 10);

    cvector_reserve(c, 10);

    printf("c capacity: %zu\n", cvector_capacity(c));
    printf("c size        : %zu\n", cvector_size(c));

    {
        int i;
        for (i = 0; i < 100; ++i) {
            cvector_push_back(c, i);
        }
    }

    printf("c capacity: %zu\n", cvector_capacity(c));
    printf("c size        : %zu\n", cvector_size(c));
    cvector_free(c);

    cvector_push_back(str_vect, convert(data));
    cvector_push_back(str_vect, strdup("Hello world"));
    cvector_push_back(str_vect, strdup("Good  bye world"));
    cvector_push_back(str_vect, strdup("not today"));

    if (str_vect) {
        size_t i;
        for (i = 0; i < cvector_size(str_vect); ++i) {
            printf("v[%zu] = %s\n", i, str_vect[i]);
        }
    }

    cvector_free_each_and_free(str_vect, free);
     cvector_vector_type(char) test           = NULL;
    cvector_push_back(test, '\0');
    
        return 0;
}
