#ifndef __ARC_H__
#define __ARC_H__

#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <stdatomic.h>

typedef _Atomic uint32_t atomic_uint32_t;
typedef void (*arc_callback)(void* ptr);

typedef struct arc {
    atomic_uint32_t     ref;            // remaining references        
    arc_callback        free;           // Callback for deallocating the struct, do nothing if `NULL`.
    arc_callback        cleanup;        // Callback for dereference sub classes, do nothing if `NULL`.
    pthread_mutex_t     mutex;          // mutex lock
} arc_t;

arc_t   arc_init(arc_callback cleanup, arc_callback free);
void    arc_destroy(void* arc);
void    arc_ref(void* arc);
void    arc_unref(void* arc);
int     arc_get_ref(void* arc);
void    arc_lock(void* arc);
void    arc_unlock(void* arc);

#endif  // __ARC_H__



#ifdef ARC_IMPLEMENTATION

extern inline arc_t arc_init(arc_callback free, arc_callback cleanup) {
    arc_t res = {
        .ref        = 1,
        .free       = free,
        .cleanup    = cleanup,
    };
    pthread_mutex_init(&res.mutex, NULL);
    return res;
}

extern inline void arc_destroy(void* arc) {
    arc_t* arc_temp = arc;
    pthread_mutex_destroy(&arc_temp->mutex);
}

extern inline void arc_ref(void* arc) {
    arc_t* arc_temp = arc;
    atomic_fetch_add(&arc_temp->ref, 1);
}

extern inline void arc_unref(void* arc) {
    arc_t* arc_temp = arc;
    if (atomic_fetch_sub(&arc_temp->ref, 1) == 1) {
        // reference goes to zero, clean up
        pthread_mutex_destroy(&arc_temp->mutex);
        if (arc_temp->cleanup != NULL) {
            arc_temp->cleanup(arc);
        }
        if (arc_temp->free != NULL) {
            arc_temp->free(arc);
        }
    }
}

extern inline int arc_get_ref(void* arc) {
    arc_t* arc_temp = arc;
    return (int) arc_temp->ref;
}

extern inline void arc_lock(void* arc) {
    arc_t* arc_temp = arc;
    pthread_mutex_lock(&arc_temp->mutex);
}

extern inline void arc_unlock(void* arc) {
    arc_t* arc_temp = arc;
    pthread_mutex_unlock(&arc_temp->mutex);
}

#endif  // ARC_IMPLEMENTATION

