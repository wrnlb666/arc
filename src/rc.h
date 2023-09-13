#ifndef __RC_H__
#define __RC_H__

#include <stdlib.h>
#include <stdint.h>

typedef void (*rc_callback)(void* ptr);

typedef struct rc {
    uint32_t    ref;
    rc_callback callback;
} rc_t;

rc_t    rc_new(rc_callback callback);
void    rc_ref(void* rc);
void    rc_unref(void* rc);
int     rc_get_ref(void* rc);

#endif  // __ARC_H__


#ifdef RC_IMPLEMENTATION

extern inline rc_t rc_new(rc_callback callback) {
    rc_t res = {
        .ref        = 1,
        .callback   = callback,
    };
    return res;
}

extern inline void rc_ref(void* rc) {
    rc_t* rc_temp = rc;
    rc_temp->ref += 1;
}

extern inline void rc_unref(void* rc) {
    rc_t* rc_temp = rc;
    if (--rc_temp->ref == 0) {
        if (rc_temp->callback == NULL) {
            free(rc);
            return;
        }
        rc_temp->callback(rc);
    }
}

extern inline int rc_get_ref(void* rc) {
    rc_t* rc_temp = rc;
    return (int) rc_temp->ref;
}

#endif  // RC_IMPLEMENTATION

