#include "../../public/array/array.h"

void init_array(FArray* c_array)
{
    c_array->size = 0;
    c_array->array_data = 0;
}

void destroy_array(FArray* c_array)
{
    assert(c_array);

    free(c_array->array_data);
}

void add_array(FArray* c_array, char const* element)
{
    assert(c_array);

    int index = c_array->size;
    c_array->size++;
    c_array->array_data = realloc(c_array->array_data, c_array->size * sizeof(FStringData));

    strcpy(c_array->array_data[index].str_data, element);
}

void printf_array(FArray* c_array)
{
    assert(c_array);

    for (int i = 0; i < c_array->size; i++)
    {
        printf("%s \r\n", c_array->array_data[i].str_data);
    }
}

char* get_array_element(FArray* c_array, int index)
{
    return c_array->array_data[index].str_data;
}

bool array_element_exist(FArray* c_array, char const* element)
{
    for (int i = 0; i < c_array->size; i++)
    {
        if (strcmp(c_array->array_data[i].str_data, element) == 0)
        {
            return true;
        }
    }

    return false;
}
