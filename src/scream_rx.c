#include "scream_rx.h"
#include "jbuf.h"
#include <utils.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

//#define LOCAL_DEBUG
#ifdef LOCAL_DEBUG
#	define TRACE(args) (printf("SCR: "), printf args)
#else
#	define TRACE(args)
#endif

enum { SCREAM_PKT_SIZE = 1157 };


static bool remaining_sample = false;
static short remaining_sample_value = 0;


/** Defines the Scream frame header */
struct scream_header {
    uint8_t rate;
    uint8_t width;
    uint8_t channels;
    uint8_t channelsMapLsb;
    uint8_t channelsMapMsb;
};

short samples[(SCREAM_PKT_SIZE - sizeof(struct scream_header))/2];


void on_scream_server_recv(char *pusrdata, unsigned short len) {
    /** \todo check source IP:port to block simultaneous transmissions */
    if (len != SCREAM_PKT_SIZE) {
        return;
    }
#if 0
    printf(".");
    fflush(stdout);
#endif
    jbuf_eop();
    // copy to align
    memcpy(samples, pusrdata + sizeof(struct scream_header), sizeof(samples));
    if (remaining_sample) {
        if (jbuf_put(remaining_sample) == 0) {
            remaining_sample = false;
        } else {
            TRACE(("Overflow: remaining sample\n"));
            return;
        }
    }
    for (int i=0; i<ARRAY_SIZE(samples); i++) {
        if (jbuf_put(samples[i])) {
            TRACE(("Overflow @%d\n", i));
            if (i%2) {
                // store sample to avoid channel swapping
                remaining_sample = true;
                remaining_sample_value = samples[i];
            }
            break;
        }
    }
}
