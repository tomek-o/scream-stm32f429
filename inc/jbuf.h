#ifndef JBUF_H
#define JBUF_H

#include "main.h"

enum {
    JBUF_FRAME_SIZE = 1152 /* scream data size */ / sizeof(short)
};

/** \return non-zero on overflow */
int jbuf_put(short sample);
/** \brief Mark End-Of-Packet */
void jbuf_eop(void);
short* jbuf_get(void);
unsigned int jbuf_available(void);

#endif
