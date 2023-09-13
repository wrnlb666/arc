#ifndef __RC_H__
#define __RC_H__

#include <stdlib.h>
#include <stdint.h>

typedef void (*rc_callback)(void* ptr);

typedef struct rc {
    uint32_t    ref;
    rc_callback callback;
    rc_callback free;
} rc_t;

rc_t    rc_init(rc_callback callback, rc_callback free);
void    rc_destroy(void* rc);
void    rc_ref(void* rc);
void    rc_unref(void* rc);
int     rc_get_ref(void* rc);

#endif  // __ARC_H__


#ifdef RC_IMPLEMENTATION

extern inline rc_t rc_init(rc_callback callback, rc_callback free) {
    rc_t res = {
        .ref        = 1,
        .callback   = callback,
        .free       = free,
    };
    return res;
}

extern inline void rc_destroy(void* rc) {
    (void) rc;
}

extern inline void rc_ref(void* rc) {
    rc_t* rc_temp = rc;
    rc_temp->ref += 1;
}

extern inline void rc_unref(void* rc) {
    rc_t* rc_temp = rc;
    if (--rc_temp->ref == 0) {
        if (rc_temp->callback != NULL) {
            rc_temp->callback(rc);
        }
        if (rc_temp->free == NULL) {
            free(rc);
            return;
        }
        rc_temp->free(rc);
    }
}

extern inline int rc_get_ref(void* rc) {
    rc_t* rc_temp = rc;
    return (int) rc_temp->ref;
}

#endif  // RC_IMPLEMENTATION

