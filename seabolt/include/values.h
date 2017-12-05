/*
 * Copyright (c) 2002-2017 "Neo Technology,"
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

#ifndef SEABOLT_VALUES
#define SEABOLT_VALUES

#include <assert.h>
#include <limits.h>
#include <memory.h>
#include <stdint.h>
#include <string.h>

#include "mem.h"

#if CHAR_BIT != 8
#error "Cannot compile if `char` is not 8-bit"
#endif

#define sizeof_n(type, n) (size_t)((n) >= 0 ? sizeof(type) * (n) : 0)

#define to_bit(x) (char)((x) == 0 ? 0 : 1);


typedef enum
{
    BOLT_NULL,                          /* ALSO IN BOLT v1 (as Null) */
    BOLT_LIST,                          /* ALSO IN BOLT v1 (as List) */
    BOLT_BIT,                           /* ALSO IN BOLT v1 (as Boolean) */
    BOLT_BYTE,
    BOLT_CHAR16,
    BOLT_CHAR32,
    BOLT_UTF8,                          /* ALSO IN BOLT v1 (as String) */
    BOLT_UTF8_DICTIONARY,               /* ALSO IN BOLT v1 (as Map) */
    BOLT_UTF16,
    BOLT_UTF16_DICTIONARY,
    BOLT_NUM8,
    BOLT_NUM16,
    BOLT_NUM32,
    BOLT_NUM64,
    BOLT_INT8,
    BOLT_INT16,
    BOLT_INT32,
    BOLT_INT64,                         /* ALSO IN BOLT v1 (as Integer) */
    BOLT_FLOAT32,
    BOLT_FLOAT32_PAIR,
    BOLT_FLOAT32_TRIPLE,
    BOLT_FLOAT32_QUAD,
    BOLT_FLOAT64,                       /* ALSO IN BOLT v1 (as Float) */
    BOLT_FLOAT64_PAIR,
    BOLT_FLOAT64_TRIPLE,
    BOLT_FLOAT64_QUAD,
    BOLT_STRUCTURE,                     /* ALSO IN BOLT v1 (as Structure) */
    BOLT_REQUEST,  // requests match 10xxxxxx
    BOLT_SUMMARY,  // summaries match 11xxxxxx
} BoltType;

struct array_t;

typedef union
{
    void* as_ptr;
    char* as_char;
    uint8_t* as_uint8;
    uint16_t* as_uint16;
    uint32_t* as_uint32;
    uint64_t* as_uint64;
    int8_t* as_int8;
    int16_t* as_int16;
    int32_t* as_int32;
    int64_t* as_int64;
    float* as_float;
    double* as_double;
    struct _BoltValue* as_value;
    struct array_t* as_array;
} data_t;

struct array_t
{
    int32_t size;
    data_t data;
};

struct _BoltValue
{
    char type;
    char is_array;
    int16_t code;
    int32_t size;               // logical size
    size_t data_size;           // physical size
    union
    {
        char as_char[16];
        uint8_t as_uint8[16];
        uint16_t as_uint16[8];
        uint32_t as_uint32[4];
        uint64_t as_uint64[2];
        int8_t as_int8[16];
        int16_t as_int16[8];
        int32_t as_int32[4];
        int64_t as_int64[2];
        float as_float[4];
        double as_double[2];
        data_t extended;
    } data;
};

typedef struct _BoltValue BoltValue;


BoltValue* BoltValue_create();

void BoltValue_toNull(BoltValue* value);

void BoltValue_toList(BoltValue* value, int32_t size);

void BoltValue_toBit(BoltValue* value, char x);

void BoltValue_toByte(BoltValue* value, char x);

void BoltValue_toBitArray(BoltValue* value, char* array, int32_t size);

void BoltValue_toByteArray(BoltValue* value, char* array, int32_t size);

void BoltValue_toChar16(BoltValue* value, uint16_t x);

void BoltValue_toChar32(BoltValue* value, uint32_t x);

void BoltValue_toChar16Array(BoltValue* value, uint16_t* array, int32_t size);

void BoltValue_toChar32Array(BoltValue* value, uint32_t* array, int32_t size);

void BoltValue_toUTF8(BoltValue* value, char* string, int32_t size);

void BoltValue_toUTF16(BoltValue* value, uint16_t* string, int32_t size);

void BoltValue_toUTF8Array(BoltValue* value, int32_t size);

void BoltValue_toUTF16Array(BoltValue* value, int32_t size);

void BoltValue_toUTF8Dictionary(BoltValue* value, int32_t size);

void BoltValue_toUTF16Dictionary(BoltValue* value, int32_t size);

void BoltValue_toNum8(BoltValue* value, uint8_t x);

void BoltValue_toNum16(BoltValue* value, uint16_t x);

void BoltValue_toNum32(BoltValue* value, uint32_t x);

void BoltValue_toNum64(BoltValue* value, uint64_t x);

void BoltValue_toNum8Array(BoltValue* value, uint8_t* array, int32_t size);

void BoltValue_toNum16Array(BoltValue* value, uint16_t* array, int32_t size);

void BoltValue_toNum32Array(BoltValue* value, uint32_t* array, int32_t size);

void BoltValue_toNum64Array(BoltValue* value, uint64_t* array, int32_t size);

void BoltValue_toInt8(BoltValue* value, int8_t x);

void BoltValue_toInt16(BoltValue* value, int16_t x);

void BoltValue_toInt32(BoltValue* value, int32_t x);

void BoltValue_toInt64(BoltValue* value, int64_t x);

void BoltValue_toInt8Array(BoltValue* value, int8_t* array, int32_t size);

void BoltValue_toInt16Array(BoltValue* value, int16_t* array, int32_t size);

void BoltValue_toInt32Array(BoltValue* value, int32_t* array, int32_t size);

void BoltValue_toInt64Array(BoltValue* value, int64_t* array, int32_t size);

void BoltValue_toFloat32(BoltValue* value, float x);

void BoltValue_toFloat32Pair(BoltValue* value, float x, float y);

void BoltValue_toFloat32Triple(BoltValue* value, float x, float y, float z);

void BoltValue_toFloat32Quad(BoltValue* value, float x, float y, float z, float a);

void BoltValue_toFloat32Array(BoltValue* value, float* array, int32_t size);

void BoltValue_toFloat32PairArray(BoltValue* value, int32_t size);

void BoltValue_toFloat32TripleArray(BoltValue* value, int32_t size);

void BoltValue_toFloat32QuadArray(BoltValue* value, int32_t size);

void BoltValue_toFloat64(BoltValue* value, double x);

void BoltValue_toFloat64Pair(BoltValue* value, double x, double y);

void BoltValue_toFloat64Triple(BoltValue* value, double x, double y, double z);

void BoltValue_toFloat64Quad(BoltValue* value, double x, double y, double z, double a);

void BoltValue_toFloat64Array(BoltValue* value, double* array, int32_t size);

void BoltValue_toFloat64PairArray(BoltValue* value, int32_t size);

void BoltValue_toFloat64TripleArray(BoltValue* value, int32_t size);

void BoltValue_toFloat64QuadArray(BoltValue* value, int32_t size);

void BoltValue_toStructure(BoltValue* value, int16_t code, int32_t size);

void BoltValue_toRequest(BoltValue* value, int16_t code, int32_t size);

void BoltValue_toSummary(BoltValue* value, int16_t code, int32_t size);

void BoltValue_toStructureArray(BoltValue* value, int16_t code, int32_t size);

BoltType BoltValue_type(const BoltValue* value);

int BoltValue_isArray(const BoltValue* value);

void BoltValue_destroy(BoltValue* value);



void BoltList_resize(BoltValue* value, int32_t size);

BoltValue* BoltList_at(const BoltValue* value, int32_t index);



char BoltBit_get(const BoltValue* value);

char BoltByte_get(const BoltValue* value);

char BoltBitArray_get(const BoltValue* value, int32_t index);

char BoltByteArray_get(const BoltValue* value, int32_t index);



uint16_t BoltChar16_get(const BoltValue* value);

uint32_t BoltChar32_get(const BoltValue* value);

uint16_t BoltChar16Array_get(const BoltValue* value, int32_t index);

uint32_t BoltChar32Array_get(const BoltValue* value, int32_t index);



const char* BoltUTF8_get(const BoltValue* value);

uint16_t* BoltUTF16_get(BoltValue* value);

char* BoltUTF8Array_get(BoltValue* value, int32_t index);

uint16_t* BoltUTF16Array_get(const BoltValue* value, int32_t index);

void BoltUTF8Array_put(BoltValue* value, int32_t index, char* string, int32_t size);

void BoltUTF16Array_put(BoltValue* value, int32_t index, uint16_t* string, int32_t size);

int32_t BoltUTF8Array_getSize(BoltValue* value, int32_t index);

int32_t BoltUTF16Array_getSize(BoltValue* value, int32_t index);

BoltValue* BoltUTF8Dictionary_getKey(const BoltValue* value, int32_t index);

BoltValue* BoltUTF16Dictionary_getKey(const BoltValue* value, int32_t index);

BoltValue* BoltUTF8Dictionary_withKey(BoltValue* value, int32_t index, char* key, int32_t key_size);

BoltValue* BoltUTF16Dictionary_withKey(BoltValue* value, int32_t index, uint16_t* key, int32_t key_size);

BoltValue* BoltUTF8Dictionary_at(const BoltValue* value, int32_t index);

BoltValue* BoltUTF16Dictionary_at(const BoltValue* value, int32_t index);

void BoltUTF8Dictionary_resize(BoltValue* value, int32_t size);

void BoltUTF16Dictionary_resize(BoltValue* value, int32_t size);



uint8_t BoltNum8_get(const BoltValue* value);

uint16_t BoltNum16_get(const BoltValue* value);

uint32_t BoltNum32_get(const BoltValue* value);

uint64_t BoltNum64_get(const BoltValue* value);

uint8_t BoltNum8Array_get(const BoltValue* value, int32_t index);

uint16_t BoltNum16Array_get(const BoltValue* value, int32_t index);

uint32_t BoltNum32Array_get(const BoltValue* value, int32_t index);

uint64_t BoltNum64Array_get(const BoltValue* value, int32_t index);



int8_t BoltInt8_get(const BoltValue* value);

int16_t BoltInt16_get(const BoltValue* value);

int32_t BoltInt32_get(const BoltValue* value);

int64_t BoltInt64_get(const BoltValue* value);

int8_t BoltInt8Array_get(const BoltValue* value, int32_t index);

int16_t BoltInt16Array_get(const BoltValue* value, int32_t index);

int32_t BoltInt32Array_get(const BoltValue* value, int32_t index);

int64_t BoltInt64Array_get(const BoltValue* value, int32_t index);



struct float_pair
{
    float x;
    float y;
};

struct float_triple
{
    float x;
    float y;
    float z;
};

struct float_quad
{
    float x;
    float y;
    float z;
    float a;
};

struct double_pair
{
    double x;
    double y;
};

struct double_triple
{
    double x;
    double y;
    double z;
};

struct double_quad
{
    double x;
    double y;
    double z;
    double a;
};

float BoltFloat32_get(const BoltValue* value);

float BoltFloat32Array_get(const BoltValue* value, int32_t index);

double BoltFloat64_get(const BoltValue* value);

double BoltFloat64Array_get(const BoltValue* value, int32_t index);


int16_t BoltStructure_code(const BoltValue* value);

int16_t BoltRequest_code(const BoltValue* value);

int16_t BoltSummary_code(const BoltValue* value);

BoltValue* BoltStructure_at(const BoltValue* value, int32_t index);

BoltValue* BoltRequest_at(const BoltValue* value, int32_t index);

BoltValue* BoltSummary_at(const BoltValue* value, int32_t index);

int32_t BoltStructureArray_getSize(const BoltValue* value, int32_t index);

void BoltStructureArray_setSize(BoltValue* value, int32_t index, int32_t size);

BoltValue* BoltStructureArray_at(const BoltValue* value, int32_t array_index, int32_t structure_index);


#endif // SEABOLT_VALUES
