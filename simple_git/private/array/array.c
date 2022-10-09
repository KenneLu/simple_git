#include "../../public/array/array.h"

void init_array(FArray* c_array)
{
    c_array->size = 0;
    c_array->data = 0;
}

void destroy_array(FArray* c_array)
{
    assert(c_array);

    free(c_array->data);
}

void add_array(FArray* c_array, char const* in_data)
{
    assert(c_array);

    int index = c_array->size;
    c_array->size++;
    c_array->data = realloc(c_array->data, c_array->size * sizeof(FStringData));

    strcpy(c_array->data[index].data, in_data);
}

void printf_array(FArray* c_array)
{
    assert(c_array);

    for (int i = 0; i < c_array->size; i++)
    {
        printf("%s \r\n", c_array->data[i].data);
    }
}

char* get_array_element(FArray* c_array, int index)
{
    return c_array->data[index].data;
}

bool array_element_exist(FArray* c_array, char const* element)
{
    for (int i = 0; i < c_array->size; i++)
    {
        if (strcmp(c_array->data[i].data, element) == 0)
        {
            return true;
        }
    }

    return false;
}
