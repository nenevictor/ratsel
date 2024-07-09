/*
  neda - Dynamic Array library. By nenevictor (shyvikaisinlove)

  Peace! So, this is my realization of dynamic array library.
  To use this library, you should declare header of
  needed type and then declare body in implementation file.
  Before include use should implement library itself.
  In body implementation you should declare the size of the chunk
  that will be added when old piece of memory will be overflowed.
  You also shouldn't put implementations in header files.
  In header files you can declare only headers.

  For example, here is a declaration of float dynamic array:
  (main.c)
    #include "neda.h"
    NEDA_HEADER(float);
    int main(void)
    {
      neda_float *da = 0;
      neda_float__init(&da);
      // here you can do operations with float array.
      neda_float__free(&da);
      return 0;
    };
  (neda_implementation.c)
    #define NEDA_IMPLEMENTATION
    #include "neda.h"
    NEDA_BODY_IMPLEMENTATION(float, 128);

  Of course, if you want, you can place header and body declaration in main file:
  (main.c)
    #define NEDA_IMPLEMENTATION
    #include "neda.h"
    NEDA_HEADER(float);
    NEDA_BODY_IMPLEMENTATION(float, 128);
    int main(void)
    {
       neda_float *da = 0;
       neda_float__init(&da);
       // here you can do operations with float array.
       neda_float__free(&da);
       return 0;
    };

  Libary change dates:
    25.06.2024: basic functions added.
*/

#ifndef NEDA_H
#define NEDA_H

#ifdef NEDA_FORCE_INLINE
#define NEDA_INLINE inline
#else
#define NEDA_INLINE
#endif

#ifdef NEDA_STATIC
#define NEDA_DEF static
#else
#ifdef __cplusplus
#define NEDA_DEF extern "C"
#else
#define NEDA_DEF extern
#endif
#endif

#define NEDA_API NEDA_DEF NEDA_INLINE

#if !defined(NEDA_MALLOC) || !defined(NEDA_REALLOC) || !defined(NEDA_FREE)
#include <malloc.h>
#endif

#ifndef NEDA_MALLOC
#define NEDA_MALLOC(_size) malloc(_size)
#endif

#ifndef NEDA_REALLOC
#define NEDA_REALLOC(_memory, _new_size) realloc(_memory, _new_size)
#endif

#ifndef NEDA_FREE
#define NEDA_FREE(_memory) free(_memory)
#endif

#if !defined(NEDA_ASSERT)
#include <assert.h>
#define NEDA_ASSERT(_expresion) assert(_expresion)
#endif

#define NEDA_UNUSED(_value) (void)(_value)

#ifndef NEDA_TYPES
#define NEDA_TYPES

#endif

#define NEDA_CHUNK_RESERVE(_chunk_size, _buffer_size) (((size_t)((float)(_buffer_size) / (float)(_chunk_size)) + 1) * _chunk_size)
// \returns logical false, if vector is valid.
#define NEDA_VALIDATE(_dynamic_array_ptr) ((_dynamic_array_ptr->size > _dynamic_array_ptr->capacity || ((_dynamic_array_ptr->size > 1) && !_dynamic_array_ptr->data)))

NEDA_DEF void neda_memset(void *_destination, unsigned char _value, size_t _size);

#ifdef NEDA_IMPLEMENTATION
NEDA_API void neda_memset(void *_destination, unsigned char _value, size_t _size)
{
  for (size_t i = 0; i < _size; i++)
  {
    ((unsigned char *)_destination)[i] = _value;
  };
};
#endif

#define NEDA_HEADER(_type)                                                                         \
  typedef int (*neda_##_type##__compare_callback)(const _type *_a, const _type *_b);               \
  typedef void (*neda_##_type##__swap_callback)(_type * _a, _type * _b);                           \
  typedef struct neda_##_type neda_##_type;                                                        \
  NEDA_DEF void neda_##_type##__init(neda_##_type **_dynamic_array);                               \
  NEDA_DEF void neda_##_type##__reserve(neda_##_type *_dynamic_array, size_t _size);               \
  NEDA_DEF _type neda_##_type##__at(neda_##_type *_dynamic_array, size_t _index);                  \
  NEDA_DEF _type *neda_##_type##__at_ptr(neda_##_type *_dynamic_array, size_t _index);             \
  NEDA_DEF void neda_##_type##__clear(neda_##_type *_dynamic_array);                               \
  NEDA_DEF void neda_##_type##__free(neda_##_type **_dynamic_array);                               \
  NEDA_DEF void neda_##_type##__shrink(neda_##_type *_dynamic_array);                              \
  NEDA_DEF void neda_##_type##__shrink_to_fit(neda_##_type *_dynamic_array);                       \
  NEDA_DEF void neda_##_type##__push_back(neda_##_type *_dynamic_array, const _type _value);       \
  NEDA_DEF _type neda_##_type##__pop_back(neda_##_type *_dynamic_array);                           \
  NEDA_DEF void neda_##_type##__push_front(neda_##_type *_dynamic_array, _type _value);            \
  NEDA_DEF _type neda_##_type##__pop_front(neda_##_type *_dynamic_array);                          \
  NEDA_DEF void neda_##_type##__insert(neda_##_type *_dynamic_array, size_t _index, _type _value); \
  NEDA_DEF void neda_##_type##__erase(neda_##_type *_dynamic_array, size_t _index);                \
  NEDA_DEF size_t neda_##_type##__size(neda_##_type *_dynamic_array);                              \
  NEDA_DEF _type *neda_##_type##__data(neda_##_type *_dynamic_array);                              \
  NEDA_DEF void neda_##_type##__sort(                                                              \
      neda_##_type *_dynamic_array,                                                                \
      neda_##_type##__compare_callback _compare_function,                                          \
      neda_##_type##__swap_callback _swap_function);                                               \
  NEDA_DEF void neda_##_type##__swap_function_default(_type *_a, _type *_b);

#define NEDA_BODY_IMPLEMENTATION(_type, _chunk_size)                                                                \
  static const size_t NEDA_CHUNK_SIZE_##_type = _chunk_size;                                                        \
  typedef int (*neda_##_type##__compare_callback)(const _type *_a, const _type *_b);                                \
  typedef void (*neda_##_type##__swap_callback)(_type * _a, _type * _b);                                            \
  typedef struct neda_##_type                                                                                       \
  {                                                                                                                 \
    _type *data;                                                                                                    \
    size_t size, capacity;                                                                                          \
  } neda_##_type;                                                                                                   \
  NEDA_API void neda_##_type##__init(neda_##_type **_dynamic_array)                                                 \
  {                                                                                                                 \
    *_dynamic_array = NEDA_MALLOC(sizeof(neda_##_type));                                                            \
    neda_memset(*_dynamic_array, 0, sizeof(neda_##_type));                                                          \
  };                                                                                                                \
  NEDA_API void neda_##_type##__reserve(neda_##_type *_dynamic_array, size_t _size)                                 \
  {                                                                                                                 \
    NEDA_ASSERT(!NEDA_VALIDATE(_dynamic_array));                                                                    \
    if (_dynamic_array->capacity >= _size)                                                                          \
      return;                                                                                                       \
    _dynamic_array->capacity = NEDA_CHUNK_RESERVE(NEDA_CHUNK_SIZE_##_type, _size);                                  \
    _dynamic_array->data = (_type *)NEDA_REALLOC(_dynamic_array->data, sizeof(_type) * _dynamic_array->capacity);   \
  };                                                                                                                \
  NEDA_API _type neda_##_type##__at(neda_##_type *_dynamic_array, size_t _index)                                    \
  {                                                                                                                 \
    NEDA_ASSERT(!NEDA_VALIDATE(_dynamic_array));                                                                    \
    NEDA_ASSERT(_dynamic_array->size > _index);                                                                     \
    return _dynamic_array->data[_index];                                                                            \
  };                                                                                                                \
  NEDA_API _type *neda_##_type##__at_ptr(neda_##_type *_dynamic_array, size_t _index)                               \
  {                                                                                                                 \
    NEDA_ASSERT(!NEDA_VALIDATE(_dynamic_array));                                                                    \
    NEDA_ASSERT(_dynamic_array->size > _index);                                                                     \
    return &_dynamic_array->data[_index];                                                                           \
  };                                                                                                                \
  NEDA_API void neda_##_type##__clear(neda_##_type *_dynamic_array)                                                 \
  {                                                                                                                 \
    NEDA_ASSERT(!NEDA_VALIDATE(_dynamic_array));                                                                    \
    _dynamic_array->size = 0;                                                                                       \
  };                                                                                                                \
  NEDA_API void neda_##_type##__free(neda_##_type **_dynamic_array)                                                 \
  {                                                                                                                 \
    NEDA_ASSERT((void *)(*_dynamic_array));                                                                         \
    NEDA_FREE((*_dynamic_array)->data);                                                                             \
    NEDA_FREE(*_dynamic_array);                                                                                     \
    *_dynamic_array = 0;                                                                                            \
  };                                                                                                                \
  NEDA_API void neda_##_type##__shrink(neda_##_type *_dynamic_array)                                                \
  {                                                                                                                 \
    NEDA_ASSERT(!NEDA_VALIDATE(_dynamic_array));                                                                    \
    const size_t new_capacity = NEDA_CHUNK_RESERVE(NEDA_CHUNK_SIZE_##_type, _dynamic_array->size);                  \
    if (new_capacity < _dynamic_array->capacity)                                                                    \
    {                                                                                                               \
      _dynamic_array->data = (_type *)NEDA_REALLOC(_dynamic_array->data, sizeof(_type) * new_capacity);             \
      _dynamic_array->capacity = new_capacity;                                                                      \
    };                                                                                                              \
  };                                                                                                                \
  NEDA_API void neda_##_type##__shrink_to_fit(neda_##_type *_dynamic_array)                                         \
  {                                                                                                                 \
    NEDA_ASSERT(!NEDA_VALIDATE(_dynamic_array));                                                                    \
    _dynamic_array->data = (_type *)NEDA_REALLOC(_dynamic_array->data, sizeof(_type) * _dynamic_array->size);       \
    _dynamic_array->capacity = _dynamic_array->size;                                                                \
  };                                                                                                                \
  NEDA_API void neda_##_type##__push_back(neda_##_type *_dynamic_array, const _type _value)                         \
  {                                                                                                                 \
    NEDA_ASSERT(!NEDA_VALIDATE(_dynamic_array));                                                                    \
    if (_dynamic_array->size + 1 >= _dynamic_array->capacity)                                                       \
    {                                                                                                               \
      _dynamic_array->capacity = NEDA_CHUNK_RESERVE(NEDA_CHUNK_SIZE_##_type, _dynamic_array->size + 1);             \
      _dynamic_array->data = (_type *)NEDA_REALLOC(_dynamic_array->data, sizeof(_type) * _dynamic_array->capacity); \
    };                                                                                                              \
    _dynamic_array->data[_dynamic_array->size] = _value;                                                            \
    _dynamic_array->size++;                                                                                         \
  };                                                                                                                \
  NEDA_API _type neda_##_type##__pop_back(neda_##_type *_dynamic_array)                                             \
  {                                                                                                                 \
    NEDA_ASSERT(!NEDA_VALIDATE(_dynamic_array));                                                                    \
    const _type result = _dynamic_array->data[_dynamic_array->size - 1];                                            \
    neda_memset(&_dynamic_array->data[_dynamic_array->size - 1], 0, sizeof(_type));                                 \
    _dynamic_array->size--;                                                                                         \
    return result;                                                                                                  \
  };                                                                                                                \
  NEDA_API void neda_##_type##__push_front(neda_##_type *_dynamic_array, _type _value)                              \
  {                                                                                                                 \
    NEDA_ASSERT(!NEDA_VALIDATE(_dynamic_array));                                                                    \
    if (_dynamic_array->size + 1 >= _dynamic_array->capacity)                                                       \
    {                                                                                                               \
      _dynamic_array->capacity = NEDA_CHUNK_RESERVE(NEDA_CHUNK_SIZE_##_type, _dynamic_array->size + 1);             \
      _dynamic_array->data = (_type *)NEDA_REALLOC(_dynamic_array->data, sizeof(_type) * _dynamic_array->capacity); \
    };                                                                                                              \
    for (size_t i = _dynamic_array->size; i > 0; i--)                                                               \
    {                                                                                                               \
      _dynamic_array->data[i] = _dynamic_array->data[i - 1];                                                        \
    };                                                                                                              \
    _dynamic_array->data[0] = _value;                                                                               \
    _dynamic_array->size++;                                                                                         \
  };                                                                                                                \
  NEDA_API _type neda_##_type##__pop_front(neda_##_type *_dynamic_array)                                            \
  {                                                                                                                 \
    NEDA_ASSERT(!NEDA_VALIDATE(_dynamic_array));                                                                    \
    const _type result = _dynamic_array->data[0];                                                                   \
    for (size_t i = 0; i < _dynamic_array->size; i++)                                                               \
    {                                                                                                               \
      _dynamic_array->data[i] = _dynamic_array->data[i + 1];                                                        \
    };                                                                                                              \
    neda_memset(&_dynamic_array->data[_dynamic_array->size - 1], 0, sizeof(_type));                                 \
    _dynamic_array->size--;                                                                                         \
    return result;                                                                                                  \
  };                                                                                                                \
  NEDA_API void neda_##_type##__insert(neda_##_type *_dynamic_array, size_t _index, _type _value)                   \
  {                                                                                                                 \
    NEDA_ASSERT(!NEDA_VALIDATE(_dynamic_array));                                                                    \
    NEDA_ASSERT(_dynamic_array->size + 1 > _index);                                                                 \
    if (_dynamic_array->size + 1 >= _dynamic_array->capacity)                                                       \
    {                                                                                                               \
      _dynamic_array->capacity = NEDA_CHUNK_RESERVE(129, _dynamic_array->size + 1);                                 \
      _dynamic_array->data = (_type *)NEDA_REALLOC(_dynamic_array->data, sizeof(_type) * _dynamic_array->capacity); \
    };                                                                                                              \
    for (size_t i = _dynamic_array->size; i > _index; i--)                                                          \
    {                                                                                                               \
      _dynamic_array->data[i] = _dynamic_array->data[i - 1];                                                        \
    };                                                                                                              \
    _dynamic_array->data[_index] = _value;                                                                          \
    _dynamic_array->size++;                                                                                         \
  };                                                                                                                \
  NEDA_API void neda_##_type##__erase(neda_##_type *_dynamic_array, size_t _index)                                  \
  {                                                                                                                 \
    NEDA_ASSERT(!NEDA_VALIDATE(_dynamic_array));                                                                    \
    NEDA_ASSERT(_dynamic_array->size > _index);                                                                     \
    for (size_t i = _index; i < _dynamic_array->size; i++)                                                          \
    {                                                                                                               \
      _dynamic_array->data[i] = _dynamic_array->data[i + 1];                                                        \
    };                                                                                                              \
    neda_memset(&_dynamic_array->data[_dynamic_array->size - 1], 0, sizeof(_type));                                 \
    _dynamic_array->size--;                                                                                         \
  };                                                                                                                \
  NEDA_API size_t neda_##_type##__size(neda_##_type *_dynamic_array)                                                \
  {                                                                                                                 \
    NEDA_ASSERT(!NEDA_VALIDATE(_dynamic_array));                                                                    \
    return _dynamic_array->size;                                                                                    \
  };                                                                                                                \
  NEDA_API _type *neda_##_type##__data(neda_##_type *_dynamic_array)                                                \
  {                                                                                                                 \
    return _dynamic_array->data;                                                                                    \
  };                                                                                                                \
  NEDA_API void neda_##_type##__sort(                                                                               \
      neda_##_type *_dynamic_array,                                                                                 \
      neda_##_type##__compare_callback _compare_function,                                                           \
      neda_##_type##__swap_callback _swap_function)                                                                 \
  {                                                                                                                 \
    for (size_t i_o = 0; i_o < _dynamic_array->size; i_o++)                                                         \
    {                                                                                                               \
      size_t swap_index = _dynamic_array->size - 1;                                                                 \
      for (size_t i_s = i_o; i_s < _dynamic_array->size; i_s++)                                                     \
      {                                                                                                             \
        if (_compare_function(&_dynamic_array->data[i_s], &_dynamic_array->data[swap_index]))                       \
        {                                                                                                           \
          swap_index = i_s;                                                                                         \
        };                                                                                                          \
      };                                                                                                            \
      _swap_function(&_dynamic_array->data[i_o], &_dynamic_array->data[swap_index]);                                \
    };                                                                                                              \
  };                                                                                                                \
  NEDA_API void neda_##_type##__swap_function_default(_type *_a, _type *_b)                                         \
  {                                                                                                                 \
    const _type temp = *_a;                                                                                         \
    *_a = *_b;                                                                                                      \
    *_b = temp;                                                                                                     \
  };

#endif