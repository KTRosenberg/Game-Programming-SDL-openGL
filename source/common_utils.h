#ifndef COMMON_UTILS_H
#define COMMON_UTILS_H

#ifdef __cplusplus
extern "C" 
{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <getopt.h>
#include <string.h>

// Credit to Handmade Network person for the following macros {
typedef int8_t   int8;
typedef int16_t  int16;
typedef int32_t  int32;
typedef int64_t  int64;

typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef float    float32; 
typedef double   float64;

typedef int8     i8;
typedef int16    i16;
typedef int32    i32;
typedef int64    i64;

typedef uint8    u8;
typedef uint16   u16;
typedef uint32   u32;
typedef uint64   u64;

typedef float32  f32; 
typedef float64  f64;

typedef u64 usize;


#define bytes(n) (n * 1ull)
#define kb(n) (bytes(n) * 1024ull)
#define mb(n) (kb(n) * 1024ull)
#define gb(n) (mb(n) * 1024ull)


#define foreach(i, lim) for (u64 (i) = 0; (i) < ((u64)lim); ++(i))
#define forrange(i, l, h) for (i64 (i) = (l); (i) < (h); ++(i))

// }

// based on Pervognsen bitwise utilities {
void* xmalloc(size_t num_bytes);
void* xcalloc(size_t num_elems, size_t elem_size);
void* xcalloc_1arg(size_t bytes);
void* xrealloc(void* ptr, size_t num_bytes);

#define ARENA_DEFAULT_BLOCK_SIZE (1024 * 1024)
#define ARENA_ALIGNMENT 8


#define MIN(x, y) ((x) <= (y) ? (x) : (y))
#define MAX(x, y) ((x) >= (y) ? (x) : (y))
#define CLAMP_MAX(x, max) MIN(x, max)
#define CLAMP_MIN(x, min) MAX(x, min)

#define ALIGN_DOWN(n, a) ((n) & ~((a) - 1))
#define ALIGN_UP(n, a) ALIGN_DOWN((n) + (a) - 1, (a))
#define ALIGN_DOWN_PTR(p, a) ((void *)ALIGN_DOWN((uintptr_t)(p), (a)))
#define ALIGN_UP_PTR(p, a) ((void *)ALIGN_UP((uintptr_t)(p), (a)))

typedef struct {
    char* ptr;
    char* end;
    char** blocks;
    size_t block_count;
    size_t block_cap;
} ArenaAllocator;

void ArenaAllocator_init(ArenaAllocator* arena);
void* ArenaAllocator_allocate(ArenaAllocator* arena, size_t size);
void ArenaAllocator_grow(ArenaAllocator* arena, size_t min_size);
void ArenaAllocator_delete(ArenaAllocator* arena);

#define StaticArrayCount(array) (sizeof(array) / sizeof((array)[0]))
#define PointerArraySize(ptr, count) (sizeof((*ptr)) * count)

#define cast(type_, v_) ((type_)v_)
#define deref(ptr_) (*ptr_)
//#define dref_as(ptr_, type_) *((type_*)ptr_)

void debug_print(const char* const in);

// https://stackoverflow.com/questions/3767869/adding-message-to-assert
#define ASSERT(condition,...) assert( \
    condition|| \
    (fprintf(stderr,__VA_ARGS__)&&fprintf(stderr," at %s:%d\n",__FILE__,__LINE__)) \
);

typedef void* (*Fn_MemoryAllocator)(size_t bytes);

#define PROGRAM_ARGS_COUNT (2)
extern struct option program_args[PROGRAM_ARGS_COUNT + 1];

typedef struct {
    bool verbose;
    bool hot_config;
} CommandLineArgs;

bool parse_command_line_args(CommandLineArgs* cmd, const int argc, char* argv[]);

#ifdef __cplusplus
}
#endif

// }
#endif


#ifdef COMMON_UTILS_IMPLEMENTATION

#ifdef __cplusplus
extern "C" 
{
#endif

void* xmalloc(size_t num_bytes)
{
    void* ptr = malloc(num_bytes);
    if (ptr == NULL) {
        perror("xmalloc failed");
        exit(EXIT_FAILURE);
    }

    return ptr;
}

void* xcalloc(size_t num_elems, size_t elem_size)
{
    void* ptr = calloc(num_elems, elem_size);
    if (ptr == NULL) {
        perror("xcalloc failed");
        exit(EXIT_FAILURE);
    }

    return ptr;
}

void* xcalloc_1arg(size_t bytes)
{
    return xcalloc(1, bytes);
}

void* xrealloc(void* ptr, size_t num_bytes)
{
    ptr = realloc(ptr, num_bytes);
    if (ptr == NULL) {
        perror("xrealloc failed");
        exit(EXIT_FAILURE);
    }

    return ptr;
}

void ArenaAllocator_init(ArenaAllocator* arena)
{
    arena->ptr         = NULL;
    arena->end         = NULL;
    arena->blocks      = NULL;
    arena->block_count = 0;
    arena->block_cap   = 0;
}
void* ArenaAllocator_allocate(ArenaAllocator* arena, size_t size)
{
    if (size > (size_t)(arena->end - arena->ptr)) {
        ArenaAllocator_grow(arena, size);

        assert(size <= (size_t)(arena->end - arena->ptr));
    }
    void* ptr = (void*)arena->ptr;
    arena->ptr = (char*)ALIGN_UP_PTR(arena->ptr + size, ARENA_ALIGNMENT);
    
    assert(arena->ptr <= arena->end);
    assert(ptr == ALIGN_DOWN_PTR(ptr, ARENA_ALIGNMENT));
    
    return ptr;
}

void ArenaAllocator_grow(ArenaAllocator* arena, size_t min_size)
{
    size_t size = ALIGN_UP(CLAMP_MIN(min_size, ARENA_DEFAULT_BLOCK_SIZE), ARENA_ALIGNMENT);
    arena->ptr = (char*)xmalloc(size);

    assert(arena->ptr == ALIGN_DOWN_PTR(arena->ptr, ARENA_ALIGNMENT));

    arena->end = arena->ptr + size;

    if (arena->block_count == arena->block_cap) {
        const size_t new_cap = (arena->block_cap + 1) * 2;
        arena->blocks = (char**)xrealloc(arena->blocks, new_cap * sizeof(*(arena->blocks)));
        arena->block_cap = new_cap;
    }

    arena->blocks[arena->block_count] = arena->ptr;
    
    ++arena->block_count;
}

void ArenaAllocator_delete(ArenaAllocator* arena)
{
    for (size_t i = 0; i < arena->block_count; ++i) {
        free(arena->blocks[i]);
    }
    free(arena->blocks);
}

void debug_print(const char* const in) 
{
    #ifdef DEBUG_PRINT
    puts(in);
    #endif
}

struct option program_args[PROGRAM_ARGS_COUNT + 1] = {
    {"verbose", no_argument, nullptr, 'v'},
    {"hotconfig", no_argument, nullptr, 'c'},
    {0, 0, 0, 0}
};


bool parse_command_line_args(CommandLineArgs* cmd, const int argc, char* argv[])
{
    memset(cmd, 0x00, sizeof(CommandLineArgs));
    // later
    char c = '\0';

    while ((c = getopt_long(argc, argv, "vc", program_args, nullptr)) != -1) {
        switch (c) {
        // number of additional threads
        case 'v':
            cmd->verbose = true;
            break;
        case 'c':
            cmd->hot_config = true;
            break;
        // missing arg
        case ':':
            fprintf(stderr, "%s: option '-%c' requires an argument\n",
                    argv[0], optopt);
            return false;
        // help
        case '?':

            return false;
        // invalid
        case 0:
            break;            
        // invalid
        default:
            fprintf(stderr, "%s: option `-%c' is invalid\n",
                    argv[0], optopt);
            return false;
        }
    }


    return true;

}


#ifdef __cplusplus
}
#endif

#undef COMMON_UTILS_IMPLEMENTATION

#endif

