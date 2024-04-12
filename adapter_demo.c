#include "vector.h"

int main(void)
{
    vector_t *vec;
    vector_create(vec);

    vector_set(vec, 0, TMP_REF(int, 100));

    vector_destroy(vec);

    return 0;
}
