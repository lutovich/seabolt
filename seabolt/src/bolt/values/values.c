/*
 * Copyright (c) 2002-2018 "Neo Technology,"
 * Network Engine for Objects in Lund AB [http://neotechnology.com]
 *
 * This file is part of Neo4j.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include <assert.h>
#include <memory.h>
#include <stdint.h>
#include <bolt/values.h>
#include "bolt/mem.h"


/**
 * Clean up a value for reuse.
 *
 * This sets any nested values to null.
 *
 * @param value
 */
void _recycle(struct BoltValue* value)
{
    enum BoltType type = BoltValue_type(value);
    if (type == BOLT_LIST || type == BOLT_STRUCTURE || type == BOLT_STRUCTURE_ARRAY || type == BOLT_MESSAGE)
    {
        for (long i = 0; i < value->size; i++)
        {
            BoltValue_to_Null(&value->data.extended.as_value[i]);
        }
    }
    else if (type == BOLT_STRING_ARRAY)
    {
        for (long i = 0; i < value->size; i++)
        {
            struct array_t string = value->data.extended.as_array[i];
            if (string.size > 0)
            {
                BoltMem_deallocate(string.data.as_ptr, (size_t)(string.size));
            }
        }
    }
    else if (type == BOLT_DICTIONARY)
    {
        for (long i = 0; i < 2 * value->size; i++)
        {
            BoltValue_to_Null(&value->data.extended.as_value[i]);
        }
    }
}

void _set_type(struct BoltValue* value, enum BoltType type, int16_t subtype, int32_t size)
{
    assert(type < 0x80);
    value->type = (char)(type);
    value->subtype = subtype;
    value->size = size;
}

void _format(struct BoltValue* value, enum BoltType type, int16_t subtype, int32_t size, const void* data, size_t data_size)
{
    _recycle(value);
    value->data.extended.as_ptr = BoltMem_adjust(value->data.extended.as_ptr, value->data_size, data_size);
    value->data_size = data_size;
    if (data != NULL && data_size > 0)
    {
        memcpy(value->data.extended.as_char, data, data_size);
    }
    _set_type(value, type, subtype, size);
}


/**
 * Resize a value that contains multiple sub-values.
 *
 * @param value
 * @param size
 * @param multiplier
 */
void _resize(struct BoltValue* value, int32_t size, int multiplier)
{
    if (size > value->size)
    {
        // grow physically
        size_t unit_size = sizeof(struct BoltValue);
        size_t new_data_size = multiplier * unit_size * size;
        size_t old_data_size = value->data_size;
        value->data.extended.as_ptr = BoltMem_adjust(value->data.extended.as_ptr, value->data_size, new_data_size);
        value->data_size = new_data_size;
        // grow logically
        memset(value->data.extended.as_char + old_data_size, 0, new_data_size - old_data_size);
        value->size = size;
    }
    else if (size < value->size)
    {
        // shrink logically
        for (long i = multiplier * size; i < multiplier * value->size; i++)
        {
            BoltValue_to_Null(&value->data.extended.as_value[i]);
        }
        value->size = size;
        // shrink physically
        size_t new_data_size = multiplier * sizeof_n(struct BoltValue, size);
        value->data.extended.as_ptr = BoltMem_adjust(value->data.extended.as_ptr, value->data_size, new_data_size);
        value->data_size = new_data_size;
    }
    else
    {
        // same size - do nothing
    }
}

struct BoltValue* BoltValue_create()
{
    size_t size = sizeof(struct BoltValue);
    struct BoltValue* value = BoltMem_allocate(size);
    _set_type(value, BOLT_NULL, 0, 0);
    value->data_size = 0;
    value->data.as_int64[0] = 0;
    value->data.as_int64[1] = 0;
    value->data.extended.as_ptr = NULL;
    return value;
}

void BoltValue_to_Null(struct BoltValue* value)
{
    if (BoltValue_type(value) != BOLT_NULL)
    {
        _format(value, BOLT_NULL, 0, 0, NULL, 0);
    }
}

void BoltValue_to_List(struct BoltValue* value, int32_t length)
{
    if (BoltValue_type(value) == BOLT_LIST)
    {
        BoltList_resize(value, length);
    }
    else
    {
        size_t data_size = sizeof(struct BoltValue) * length;
        _recycle(value);
        value->data.extended.as_ptr = BoltMem_adjust(value->data.extended.as_ptr, value->data_size, data_size);
        value->data_size = data_size;
        memset(value->data.extended.as_char, 0, data_size);
        _set_type(value, BOLT_LIST, 0, length);
    }
}

enum BoltType BoltValue_type(const struct BoltValue* value)
{
    return (enum BoltType)(value->type);
}

void BoltValue_destroy(struct BoltValue* value)
{
    BoltValue_to_Null(value);
    BoltMem_deallocate(value, sizeof(struct BoltValue));
}

void BoltList_resize(struct BoltValue* value, int32_t size)
{
    assert(BoltValue_type(value) == BOLT_LIST);
    _resize(value, size, 1);
}

struct BoltValue* BoltList_value(const struct BoltValue* value, int32_t index)
{
    assert(BoltValue_type(value) == BOLT_LIST);
    return &value->data.extended.as_value[index];
}

int BoltNull_write(const struct BoltValue * value, FILE * file)
{
    assert(BoltValue_type(value) == BOLT_NULL);
    fprintf(file, "null");
    return 0;
}

int BoltList_write(const struct BoltValue * value, FILE * file, int32_t protocol_version)
{
    assert(BoltValue_type(value) == BOLT_LIST);
    fprintf(file, "[");
    for (int i = 0; i < value->size; i++)
    {
        if (i > 0) fprintf(file, ", ");
        BoltValue_write(BoltList_value(value, i), file, protocol_version);
    }
    fprintf(file, "]");
    return 0;
}

int BoltValue_write(struct BoltValue * value, FILE * file, int32_t protocol_version)
{
    switch (BoltValue_type(value))
    {
        case BOLT_NULL:
            return BoltNull_write(value, file);
        case BOLT_LIST:
            return BoltList_write(value, file, protocol_version);
        case BOLT_BIT:
            return BoltBit_write(value, file);
        case BOLT_BYTE:
            return BoltByte_write(value, file);
        case BOLT_BIT_ARRAY:
            return BoltBitArray_write(value, file);
        case BOLT_BYTE_ARRAY:
            return BoltByteArray_write(value, file);
        case BOLT_CHAR:
            return BoltChar_write(value, file);
        case BOLT_CHAR_ARRAY:
            return BoltCharArray_write(value, file);
        case BOLT_STRING:
            return BoltString_write(value, file);
        case BOLT_STRING_ARRAY:
            return BoltStringArray_write(value, file);
        case BOLT_DICTIONARY:
            return BoltDictionary_write(value, file, protocol_version);
        case BOLT_INT16:
            return BoltInt16_write(value, file);
        case BOLT_INT32:
            return BoltInt32_write(value, file);
        case BOLT_INT64:
            return BoltInt64_write(value, file);
        case BOLT_INT16_ARRAY:
            return BoltInt16Array_write(value, file);
        case BOLT_INT32_ARRAY:
            return BoltInt32Array_write(value, file);
        case BOLT_INT64_ARRAY:
            return BoltInt64Array_write(value, file);
        case BOLT_FLOAT64:
            return BoltFloat64_write(value, file);
        case BOLT_FLOAT64_ARRAY:
            return BoltFloat64Array_write(value, file);
        case BOLT_STRUCTURE:
            return BoltStructure_write(value, file, protocol_version);
        case BOLT_STRUCTURE_ARRAY:
            return BoltStructureArray_write(value, file, protocol_version);
        case BOLT_MESSAGE:
            return BoltMessage_write(value, file, protocol_version);
        default:
            fprintf(file, "?");
            return -1;
    }
}
